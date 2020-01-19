/**
 * \file LCSF_Validator.c
 * \brief Light Command Set Format validator module
 * \author Jean-Roland Gosse

    This file is part of LCSF C Stack.

    LCSF C Stack is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LCSF C Stack is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this project. If not, see <https://www.gnu.org/licenses/>
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include <MemAlloc.h>
#include <Filo.h>
#include <LCSF_Transcoder.h>
#include <LCSF_Validator.h>

// *** Definitions ***
// --- Private Types ---

// Lcsf error protocol (LCSF_EP) id
#define LCSF_ERROR_PROTOCOL_ID 0xFFFF

// LCSF_EP command id enum
enum _lcsf_ep_cmd_enum {
    LCSF_EP_CMD_ERROR_ID = 0x00,
};

// LCSF_EP error command attribute number
#define LCSF_EP_CMD_ERROR_ATT_NB 2

// LCSF_EP error command attribute enum
enum _lcsf_ep_cmd_error_att_enum {
    LCSF_EP_CMD_ERROR_ATT_ERR_LOC_ID = 0x00,
    LCSF_EP_CMD_ERROR_ATT_ERR_TYPE_ID = 0x01,
};

// LCSF_EP error command attribute sizes
#define LCSF_EP_CMD_ERROR_ATT_ERR_LOC_SIZE sizeof(uint8_t)
#define LCSF_EP_CMD_ERROR_ATT_ERR_TYPE_SIZE sizeof(uint8_t)

// LCSF_EP error command attribute location values
enum _lcsf_ep_cmd_error_att_location_val_enum {
    LCSF_EP_ERROR_LOC_DECODE_ERROR = 0x00,
    LCSF_EP_ERROR_LOC_VALIDATION_ERROR = 0x01,
};

// LCSF_EP error command attribute error code values
enum _lcsf_ep_cmd_error_att_err_code_val_enum {
    LCSF_EP_ERROR_CODE_UNKNOWN_PROT_ID = 0x00,
    LCSF_EP_ERROR_CODE_UNKNOWN_CMD_ID = 0x01,
    LCSF_EP_ERROR_CODE_UNKNOWN_ATT_ID = 0x02,
    LCSF_EP_ERROR_CODE_TOO_MANY_ATT = 0x03,
    LCSF_EP_ERROR_CODE_MISS_NONOPT_ATT = 0x04,
    LCSF_EP_ERROR_CODE_WRONG_ATT_DATA_TYPE = 0x05,
    LCSF_EP_ERROR_CODE_UNKNOWN_ERROR = 0xFF,
};

// LCSF_EP error command description structure
typedef struct _lcsf_ep_cmd_error_desc {
    bool HasReceivedError;
    uint8_t ErrorLocation;
    uint8_t ErrorCode;
} lcsf_ep_cmd_error_desc_t;

// Module information structure
typedef struct _lcsf_validator_info {
    uint8_t LastErrorCode; // Contains the last error the module encountered
    lcsf_ep_cmd_error_desc_t LastReceivedError; // Contains the last received lcsf error
    filo_desc_t *pReceiverFilo; // Pointer to the receiver filo
    filo_desc_t *pSenderFilo; // Pointer to the sender filo
    const lcsf_validator_protocol_desc_t **pProtArray; // Pointer to contain the module protocol array
    const lcsf_validator_init_desc_t *pInitDesc; // Pointer to module initialization descriptor
} lcsf_validator_info_t;

// --- Private Constants ---
// --- Private Function Prototypes ---
// Utility functions
static bool LCSF_AllocateReceiverAttArray(uint16_t attNb, lcsf_valid_att_t **pAttArray);
static bool LCSF_AllocateSenderAttArray(uint16_t attNb, lcsf_raw_att_t **pAttArray);
static LCSFInterpretCallback *LCSF_GetCallback(uint16_t protId);
static const lcsf_protocol_desc_t *LCSF_GetDescriptor(uint16_t protId);
// Table look up functions
static bool LCSF_ValidateCmdId(uint16_t cmdId, uint16_t cmdNb, uint16_t *pCmdIdx, const lcsf_command_desc_t *pCmdDescArray);
static bool LCSF_HasNonOptionalAttribute(uint16_t attNb, const lcsf_attribute_desc_t *pAttDescArray);
static bool LCSF_RecogniseAttributeId(uint16_t attId, uint16_t attNb, uint16_t *pAttIdx, const lcsf_raw_att_t *pAttArray);
static uint16_t LCSF_CountAttributes(uint16_t descAttNb, lcsf_valid_att_t *pAttArray);
// Receiver functions
static bool LCSF_ValidateDataType(uint16_t dataSize, uint8_t descDataType);
static bool LCSF_ValidateAttribute_Rec(uint16_t attNb, const lcsf_raw_att_t *pAttArray, uint16_t descAttNb, const lcsf_attribute_desc_t *pAttDescArray, lcsf_valid_att_t **pValidAttArray);
static bool LCSF_ValidateAttribute(const lcsf_raw_msg_t *pMessage, const lcsf_command_desc_t *pCmdDesc, lcsf_valid_att_t **pValidAttArray);
// Sender functions
static bool LCSF_FillAttributeInfo(lcsf_raw_att_t *pRawAtt, uint8_t descDataType, uint16_t subAttNb, const lcsf_valid_att_t *pValidAtt);
static bool LCSF_FillAttribute_Rec(uint16_t descAttNb, const lcsf_attribute_desc_t *pAttDescArray, const lcsf_valid_att_t *pValidAttArray, lcsf_raw_att_t *pRawAttArray);
static bool LCSF_FillAttributeArray(uint16_t attNb, uint16_t descAttNb, const lcsf_valid_att_t *pAttArray, const lcsf_command_desc_t *pCmdDesc, lcsf_raw_att_t **pRawAttArray);
// Lcsf error processing functions
static bool LCSF_ValidatorSendError(uint8_t errorLoc, uint8_t errorType);
static bool LCSF_ProcessReceivedError(const lcsf_raw_msg_t *pErrorMsg);

// --- Private Variables ---
static lcsf_validator_info_t LcsfValidatorInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static bool LCSF_AllocateReceiverAttArray(uint16_t attNb, lcsf_valid_att_t **pAttArray)
 * \brief Allocate an attribute array for the receiver
 *
 * \param attNb number of attribute in the array
 * \param pAttArray pointer to contain the attribute array
 * \return bool: true if operation was a success
 */
static bool LCSF_AllocateReceiverAttArray(uint16_t attNb, lcsf_valid_att_t **pAttArray) {
    return FiloGet(LcsfValidatorInfo.pReceiverFilo, attNb, (void **)pAttArray);
}

/**
 * \fn static bool LCSF_AllocateSenderAttArray(uint16_t attNb, lcsf_raw_att_t **pAttArray)
 * \brief Allocate an attribute array for the sender
 *
 * \param attNb number of attribute in the array
 * \param pAttArray pointer to contain the attribute array
 * \return bool: true if operation was a success
 */
static bool LCSF_AllocateSenderAttArray(uint16_t attNb, lcsf_raw_att_t **pAttArray) {
    return FiloGet(LcsfValidatorInfo.pSenderFilo, attNb, (void **)pAttArray);
}

/**
 * \fn static LCSFInterpretCallback *LCSF_GetCallback(uint8_t protId)
 * \brief Return the corresponding protocol interpretor function
 *
 * \param protId protocol identifier
 * \return LCSFInterpretCallback *: Pointer to the interpretor callback (NULL if unknown protocol)
 */
static LCSFInterpretCallback *LCSF_GetCallback(uint16_t protId) {
    for (uint8_t idx = 0; idx < LcsfValidatorInfo.pInitDesc->ProtNb; idx++) {
        const lcsf_validator_protocol_desc_t *pProtocol = LcsfValidatorInfo.pProtArray[idx];

        if ((pProtocol != NULL) && (pProtocol->ProtId == protId)) {
            return pProtocol->pFnInterpretMsg;
        }
    }
    return NULL;
}

/**
 * \fn static const lcsf_protocol_desc_t *LCSF_GetDescriptor(uint8_t protId)
 * \brief Return the corresponding protocol descriptor
 *
 * \param protId protocol identifier
 * \return lcsf_protocol_desc_t *: Pointer to the descriptor (NULL if unknown protocol)
 */
static const lcsf_protocol_desc_t *LCSF_GetDescriptor(uint16_t protId) {
    for (uint8_t idx = 0; idx < LcsfValidatorInfo.pInitDesc->ProtNb; idx++) {
        const lcsf_validator_protocol_desc_t *pProtocol = LcsfValidatorInfo.pProtArray[idx];

        if ((pProtocol != NULL) && (pProtocol->ProtId == protId)) {
            return pProtocol->pProtDesc;
        }
    }
    return NULL;
}

/**
 * \fn static bool LCSF_ValidateCmdId(uint16_t cmdId, uint16_t cmdNb, uint16_t *pCmdIdx, const lcsf_command_desc_t *pCmdDescArray)
 * \brief Find the index in a command descriptor array corresponding to the identifier
 *
 * \param cmdId command identifier
 * \param cmdNb number of commands in the descriptor array
 * \param pCmdIdx pointer to contain the command index in the array
 * \param pCmdDescArray pointer to the command descriptor array
 * \return bool: true if operation was a success
 */
static bool LCSF_ValidateCmdId(uint16_t cmdId, uint16_t cmdNb, uint16_t *pCmdIdx, const lcsf_command_desc_t *pCmdDescArray) {

    if(pCmdDescArray == NULL) {
        return false;
    }
    for (uint16_t idx = 0; idx < cmdNb; idx++) {
        if (pCmdDescArray[idx].CmdId == cmdId) {
            *pCmdIdx = idx;
            return true;
        }
    }
    return false;
}

/**
 * \fn static bool LCSF_HasNonOptionalAttribute(uint16_t attNb, const lcsf_attribute_desc_t *pAttDescArray)
 * \brief Indicate if there is a non-optional attribute in a attribute descriptor array
 *
 * \param attNb number of attribute in the descriptor array
 * \param pAttDescArray pointer to the attribute descriptor array
 * \return bool: true if there is a non-optional attribute
 */
static bool LCSF_HasNonOptionalAttribute(uint16_t attNb, const lcsf_attribute_desc_t *pAttDescArray) {

    if (pAttDescArray == NULL) {
        return false;
    }
    for (uint16_t idx = 0; idx < attNb; idx++) {
        if (!pAttDescArray[idx].IsOptional) {
            return true;
        }
    }
    return false;
}

/**
 * \fn static bool LCSF_RecogniseAttributeId(uint16_t attId, uint16_t attNb, uint16_t *pAttIdx, lcsf_raw_att_t *pAttArray)
 * \brief Find in a received attribute array, the index corresponding to the identifier
 *
 * \param attId attribute identifier
 * \param attNb number of attributes in the array
 * \param pAttIdx pointer to contain the attribute index in the array
 * \param pAttArray pointer to the attribute array
 * \return bool: true if operation was a success
 */
static bool LCSF_RecogniseAttributeId(uint16_t attId, uint16_t attNb, uint16_t *pAttIdx, const lcsf_raw_att_t *pAttArray) {

    if(pAttArray == NULL) {
        return false;
    }
    for (uint16_t idx = 0; idx < attNb ; idx++) {
        if (pAttArray[idx].AttId == attId) {
            *pAttIdx = idx;
            return true;
        }
    }
    return false;
}

/**
 * \fn static uint16_t LCSF_CountAttributes(uint16_t descAttNb, lcsf_valid_att_t *pAttArray)
 * \brief Count the attributes in a valid attribute array
 *
 * \param descAttNb max number of attributes in the array, according to descriptor
 * \param pAttArray pointer to the attributes array
 * \return uint16_t: number of attributes
 */
static uint16_t LCSF_CountAttributes(uint16_t descAttNb, lcsf_valid_att_t *pAttArray) {
    uint16_t count = 0;

    if (pAttArray == NULL) {
        return 0;
    }
    for (uint16_t idx = 0; idx < descAttNb; idx++) {
        if (pAttArray[idx].Payload.pData != NULL) {
            count++;
        }
    }
    return count;
}

/**
 * \fn static bool LCSF_ValidateDataType(uint16_t dataSize, uint8_t descDataType)
 * \brief Validate the data size of received attribute payload
 *
 * \param dataSize size of the data
 * \param descDataType type of the data in the descriptor
 * \return bool: true if operation was a success
 */
static bool LCSF_ValidateDataType(uint16_t dataSize, uint8_t descDataType) {

    switch (descDataType) {
        case LCSF_UINT8:
            return (dataSize == sizeof(uint8_t));
        break;

        case LCSF_UINT16:
            return (dataSize == sizeof(uint16_t));
        break;

        case LCSF_UINT32:
            return (dataSize == sizeof(uint32_t));
        break;

        case LCSF_BYTE_ARRAY:
        case LCSF_STRING:
            return (dataSize != 0); // We can only check that there is data
        break;

        default: // Unknown data type
            return false;
        break;
    }
}

/**
 * \fn static bool LCSF_ValidateAttribute_Rec(uint16_t attNb, lcsf_raw_att_t *pAttArray, uint16_t descAttNb, const lcsf_attribute_desc_t *pAttDescArray, lcsf_valid_att_t **pValidAttArray)
 * \brief Recursively validate received attributes and their payload
 *
 * \param attNb number of attributes in the received array
 * \param pAttArray pointer to the received attribute array
 * \param descAttNb number of attributes in the descriptor array
 * \param pAttDescArray pointer to the attribute descriptor array
 * \param pValidAttArray pointer to contain the validated attributes array
 * \return bool: true if operation was a success
 */
static bool LCSF_ValidateAttribute_Rec(uint16_t attNb, const lcsf_raw_att_t *pAttArray, uint16_t descAttNb, const lcsf_attribute_desc_t *pAttDescArray, lcsf_valid_att_t **pValidAttArray) {

    if (pAttDescArray == NULL) {
        LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_UNKNOWN_ERROR;
        return false;
    }
    if (pAttArray == NULL) {
        // No received attributes, we check if this is normal
        if(LCSF_HasNonOptionalAttribute(descAttNb, pAttDescArray)) {
            LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_MISS_NONOPT_ATT;
            return false;
        }
    }
    if (attNb > descAttNb) {
        LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_TOO_MANY_ATT;
        return false;
    }
    // Allocate the valid attribute array
    if (!LCSF_AllocateReceiverAttArray(descAttNb, pValidAttArray)){
        LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_UNKNOWN_ERROR;
        return false;
    }
    // We note the base number of received attributes
    uint16_t rxAttNb = attNb;
    // We go through the descriptor list
    for (uint16_t idx = 0; idx < descAttNb; idx++) {
        // Intermediary variables
        lcsf_valid_att_t *pCurrValidAtt = &((*pValidAttArray)[idx]);
        const lcsf_attribute_desc_t *pCurrDescAtt = &(pAttDescArray[idx]); // Same index because both arrays should have same size
        uint16_t attIdx = 0;
        // Check if there is unprocessed received attributes and if one corresponds to current descriptor attribute
        if ((rxAttNb > 0) && (LCSF_RecogniseAttributeId(pCurrDescAtt->AttId, attNb, &attIdx, pAttArray))) {
            // Intermediary variable
            const lcsf_raw_att_t *pCurrRxAtt = &(pAttArray[attIdx]);
            // We take into account the processed attribute
            rxAttNb--;
            // Check the payload type
            if (pCurrDescAtt->DataType == LCSF_SUB_ATTRIBUTES) {
                if (pCurrRxAtt->HasSubAtt) {
                    // We process the sub-attributes
                    if (!LCSF_ValidateAttribute_Rec(pCurrRxAtt->PayloadSize, pCurrRxAtt->Payload.pSubAttArray, pCurrDescAtt->SubAttNb, pCurrDescAtt->pSubAttDescArray, &(pCurrValidAtt->Payload.pSubAttArray))) {
                        return false;
                    }
                } else if (LCSF_HasNonOptionalAttribute(pCurrDescAtt->SubAttNb, pCurrDescAtt->pSubAttDescArray)) {
                    // Missing sub-attributes
                    LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_MISS_NONOPT_ATT;
                    return false;
                }
            } else {
                // Check if data type is valid
                if (!LCSF_ValidateDataType(pCurrRxAtt->PayloadSize, pCurrDescAtt->DataType)) {
                    LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_WRONG_ATT_DATA_TYPE;
                    return false;
                }
                // Note the data and data size
                pCurrValidAtt->Payload.pData = pCurrRxAtt->Payload.pData;
                pCurrValidAtt->PayloadSize = pCurrRxAtt->PayloadSize;
            }
        } else if (!pCurrDescAtt->IsOptional) {
            // Missing non-optional attribute, error
            LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_MISS_NONOPT_ATT;
            return false;
        } else {
            // Missing optional attribute, valid case
            pCurrValidAtt->Payload.pData = NULL;
        }
    }
    // Check if we processed everything
    if (rxAttNb > 0) {
        LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_UNKNOWN_ATT_ID;
        return false;
    }
    return true;
}

/**
 * \fn static bool LCSF_ValidateAttribute(const lcsf_raw_msg_t *pMessage, const lcsf_command_desc_t *pCmdDesc, lcsf_valid_att_t **pValidAttArray)
 * \brief Validate the attributes of a received message
 *
 * \param pMessage pointer to the received message
 * \param pCmdDesc pointer to the command descriptor
 * \param pValidAttArray pointer to contain the validated attributes array
 * \return bool: true if operation was a success
 */
static bool LCSF_ValidateAttribute(const lcsf_raw_msg_t *pMessage, const lcsf_command_desc_t *pCmdDesc, lcsf_valid_att_t **pValidAttArray) {

    if ((pCmdDesc->AttNb == 0) && (pMessage->AttNb > 0)) {
        // We received attributes while the descriptor says we expect none
        LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_TOO_MANY_ATT;
        return false;
    } else if ((pCmdDesc->AttNb != 0) && (pMessage->AttNb == 0)) {
        // We received no attributes, we check if this is normal or if there is missing attributes
        if (LCSF_HasNonOptionalAttribute(pCmdDesc->AttNb, pCmdDesc->pAttDescArray)) {
            LcsfValidatorInfo.LastErrorCode = LCSF_EP_ERROR_CODE_MISS_NONOPT_ATT;
            return false;
        }
    } else if ((pCmdDesc->AttNb != 0) && (pCmdDesc->pAttDescArray != NULL)) {
        // We validate the attribute array
        return LCSF_ValidateAttribute_Rec(pMessage->AttNb, pMessage->pAttArray, pCmdDesc->AttNb, pCmdDesc->pAttDescArray, pValidAttArray);
    }
    return true;
}

/**
 * \fn static bool LCSF_FillAttributeInfo(lcsf_raw_att_t *pRawAtt, uint8_t descDataType, uint16_t subAttNb, const lcsf_valid_att_t *pValidAtt)
 * \brief Fill a raw attribute info with data from a valid attribute and descriptor
 *
 * \param pRawAtt pointer to the raw attribute to fill
 * \param descDataType data type from the descriptor
 * \param subAttNb number of sub attribute
 * \param pValidAtt pointer to the valid attribute containing the info
 * \return bool: true if operation was a success
 */
static bool LCSF_FillAttributeInfo(lcsf_raw_att_t *pRawAtt, uint8_t descDataType, uint16_t subAttNb, const lcsf_valid_att_t *pValidAtt) {
    uint16_t stringSize = 0;

    switch (descDataType) {
        case LCSF_UINT8:
              pRawAtt->PayloadSize = sizeof(uint8_t);
              pRawAtt->HasSubAtt = false;
              return true;
        break;

        case LCSF_UINT16:
              pRawAtt->PayloadSize = sizeof(uint16_t);
              pRawAtt->HasSubAtt = false;
              return true;
        break;

        case LCSF_UINT32:
              pRawAtt->PayloadSize = sizeof(uint32_t);
              pRawAtt->HasSubAtt = false;
              return true;
        break;

        case LCSF_BYTE_ARRAY:
              if (pValidAtt->PayloadSize > 0) {
                  pRawAtt->PayloadSize = (uint16_t)pValidAtt->PayloadSize;
                  pRawAtt->HasSubAtt = false;
              return true;
              } else {
                  return false;
              }
        break;

        case LCSF_STRING:
              stringSize = (uint16_t)strlen((char *)pValidAtt->Payload.pData);
              if (stringSize > 0) {
                  pRawAtt->PayloadSize = stringSize + 1;
                  pRawAtt->HasSubAtt = false;
                  return true;
              } else {
                  return false;
              }
        break;

        case LCSF_SUB_ATTRIBUTES:
              pRawAtt->PayloadSize = subAttNb;
              pRawAtt->HasSubAtt = true;
              return true;
        break;

          default:
              return false;
        break;
    }
    return false;
}

/**
 * \fn static bool LCSF_FillAttribute_Rec(uint16_t descAttNb, const lcsf_attribute_desc_t *pAttDescArray, const lcsf_valid_att_t *pValidAttArray, lcsf_raw_att_t *pRawAttArray)
 * \brief Fill recursively a raw attribute array with data from a valid and descriptor attribute array
 * \param descAttNb number of attributes in the descriptor array
 * \param pAttDescArray pointer to the attribute descriptor array
 * \param pValidAttArray pointer to the attribute array to send
 * \param pRawAttArray pointer to the raw attributes array to fill
 * \return bool: true if operation was a success
 */
static bool LCSF_FillAttribute_Rec(uint16_t descAttNb, const lcsf_attribute_desc_t *pAttDescArray, const lcsf_valid_att_t *pValidAttArray, lcsf_raw_att_t *pRawAttArray) {
    // Raw attribute array index
    uint16_t fillIdx = 0;

    if ((pAttDescArray == NULL) || (pValidAttArray == NULL)) {
        return false;
    }
    // Go through the valid attribute array
    for (uint16_t idx = 0; idx < descAttNb; idx++) {
        // Intermediary variables
        const lcsf_valid_att_t *pCurrValidAtt = &(pValidAttArray[idx]);
        const lcsf_attribute_desc_t *pCurrAttDesc = &(pAttDescArray[idx]); // Same index because both arrays should have same size
        lcsf_raw_att_t *pCurrRawAtt = &(pRawAttArray[fillIdx]);
        // Check if attribute is here or not
        if (pCurrValidAtt->Payload.pData == NULL) { // Same as (pCurrValidAtt->Payload.pSubAttArray == NULL) as it's an union
            // Check if attribute is optional
            if (!pCurrAttDesc->IsOptional) {
                return false;
            }
        } else { // Attribute is here
            uint16_t subAttNb = 0;
            if (pCurrAttDesc->DataType == LCSF_SUB_ATTRIBUTES) {
                // Count the sub-attributes
                subAttNb = LCSF_CountAttributes(pCurrAttDesc->SubAttNb, pCurrValidAtt->Payload.pSubAttArray);
            }
            // Fill attribute info
            if (!LCSF_FillAttributeInfo(pCurrRawAtt, pCurrAttDesc->DataType, subAttNb, pCurrValidAtt)) {
                return false;
            }
            // Note the attribute id
            pCurrRawAtt->AttId = pCurrAttDesc->AttId;
            // Check if attribute has sub-attributes or data
            if (pCurrRawAtt->HasSubAtt) {
                if (subAttNb == 0) {
                    return false;
                }
                // Allocate sub-attribute array
                if (!LCSF_AllocateSenderAttArray(pCurrRawAtt->PayloadSize, &(pCurrRawAtt->Payload.pSubAttArray))) {
                    return false;
                }
                // Fill sub-attribute array
                if (!LCSF_FillAttribute_Rec(pCurrAttDesc->SubAttNb, pCurrAttDesc->pSubAttDescArray, pCurrValidAtt->Payload.pSubAttArray, pCurrRawAtt->Payload.pSubAttArray)) {
                    return false;
                }
            } else { // Attribute has data
                // Note data address
                pCurrRawAtt->Payload.pData = pCurrValidAtt->Payload.pData;
            }
            // Increase raw attribute array index
            fillIdx++;
        }
    }
    return true;
}

/**
 * \fn static bool LCSF_FillAttributeArray(uint16_t attNb, const lcsf_valid_att_t *pAttArray, const lcsf_command_desc_t *pCmdDesc, lcsf_raw_att_t **pRawAttArray)
 * \brief Fill a raw attribute array with data from a valid attribute array and a command descriptor
 *
 * \param attNb number of attributes of the command to send
 * \param descAttNb number of attributes in the descriptor array
 * \param pAttArray pointer to the attribute array to send
 * \param pCmdDesc pointer to the command descriptor
 * \param pRawAttArray pointer to contain the raw attributes array
 * \return bool: true if operation was a success
 */
static bool LCSF_FillAttributeArray(uint16_t attNb, uint16_t descAttNb, const lcsf_valid_att_t *pAttArray, const lcsf_command_desc_t *pCmdDesc, lcsf_raw_att_t **pRawAttArray) {

    // Try to allocate the attribute list
    if (!LCSF_AllocateSenderAttArray(attNb, pRawAttArray)) {
        return false;
    }
    // Fill the attribute list
    return LCSF_FillAttribute_Rec(pCmdDesc->AttNb, pCmdDesc->pAttDescArray, pAttArray, *pRawAttArray);
}

/**
 * \fn static bool LCSF_ValidatorSendError(uint8_t errorLoc, uint8_t errorType)
 * \brief Send an lcsf error message
 *
 * \param errorLoc location of the error encountered
 * \param errorType type of the error encountered
 * \return bool: true if operation was a success
 */
static bool LCSF_ValidatorSendError(uint8_t errorLoc, uint8_t errorType) {
    lcsf_raw_msg_t errorMsg;
    lcsf_raw_att_t msgAttArray[LCSF_EP_CMD_ERROR_ATT_NB];
    uint8_t errorLocVal = errorLoc;
    uint8_t errorCodeVal = errorType;
    // Fill message information
    errorMsg.ProtId = LCSF_ERROR_PROTOCOL_ID;
    errorMsg.CmdId = LCSF_EP_CMD_ERROR_ID;
    errorMsg.AttNb = LCSF_EP_CMD_ERROR_ATT_NB;
    errorMsg.pAttArray = msgAttArray;
    // Fill error location attribute
    msgAttArray[0].AttId = LCSF_EP_CMD_ERROR_ATT_ERR_LOC_ID;
    msgAttArray[0].PayloadSize = LCSF_EP_CMD_ERROR_ATT_ERR_LOC_SIZE;
    msgAttArray[0].HasSubAtt = false;
    msgAttArray[0].Payload.pData = &errorLocVal;
    // Fill error code attribute
    msgAttArray[1].AttId = LCSF_EP_CMD_ERROR_ATT_ERR_TYPE_ID;
    msgAttArray[1].PayloadSize = LCSF_EP_CMD_ERROR_ATT_ERR_TYPE_SIZE;
    msgAttArray[1].HasSubAtt = false;
    msgAttArray[1].Payload.pData = &errorCodeVal;
    // Send the message to transcoder
    return LCSF_TranscoderSend(&errorMsg);
}

/**
 * \fn static bool LCSF_ProcessReceivedError(const lcsf_raw_msg_t *pErrorMsg)
 * \brief Process incoming lcsf error message validate and save them
 *
 * \param pErrorMsg pointer to the error message
 * \return bool: true if operation was a success
 */
static bool LCSF_ProcessReceivedError(const lcsf_raw_msg_t *pErrorMsg) {
    // Validate message attribute number
    if (pErrorMsg->AttNb != LCSF_EP_CMD_ERROR_ATT_NB) {
        return false;
    }
    // Validate attributes data size
    if ((pErrorMsg->pAttArray[0].PayloadSize != sizeof(uint8_t)) || (pErrorMsg->pAttArray[1].PayloadSize != sizeof(uint8_t))) {
        return false;
    }
    // Save error message information
    LcsfValidatorInfo.LastReceivedError.HasReceivedError = true;
    LcsfValidatorInfo.LastReceivedError.ErrorLocation = *(pErrorMsg->pAttArray[0].Payload.pData);
    LcsfValidatorInfo.LastReceivedError.ErrorCode = *(pErrorMsg->pAttArray[1].Payload.pData);
    return true;
}

// *** Public Functions ***

bool LCSF_ValidatorInit(const lcsf_validator_init_desc_t *pInitDesc) {

    if (pInitDesc == NULL) {
        return false;
    }
    // Note initialization descriptor
    LcsfValidatorInfo.pInitDesc = pInitDesc;
    // Allocate structures
    LcsfValidatorInfo.pProtArray = MemAllocCalloc((uint32_t)(pInitDesc->ProtNb * sizeof(lcsf_validator_protocol_desc_t *)));
    LcsfValidatorInfo.pSenderFilo = FiloCreate(LcsfValidatorInfo.pInitDesc->FiloSize, sizeof(lcsf_valid_att_t));
    LcsfValidatorInfo.pReceiverFilo = FiloCreate(LcsfValidatorInfo.pInitDesc->FiloSize, sizeof(lcsf_raw_att_t));
    // Initialize variables
    LcsfValidatorInfo.LastReceivedError.HasReceivedError = false;
    LcsfValidatorInfo.LastReceivedError.ErrorLocation = 0;
    LcsfValidatorInfo.LastReceivedError.ErrorCode = 0;
    return true;
}

bool LCSF_ValidatorAddProtocol(uint8_t protIdx, const lcsf_validator_protocol_desc_t *pProtDesc) {
    if ((pProtDesc != NULL) && (protIdx < LcsfValidatorInfo.pInitDesc->ProtNb)) {
        LcsfValidatorInfo.pProtArray[protIdx] = pProtDesc;
        return true;
    } else {
        return false;
    }
}

bool LCSF_ValidatorSendTranscoderError(uint8_t errorCode) {
    return LCSF_ValidatorSendError(LCSF_EP_ERROR_LOC_DECODE_ERROR, errorCode);
}

bool LCSF_ValidatorReceive(const lcsf_raw_msg_t *pMessage) {

    if (pMessage == NULL) {
        return false;
    }
    // Process lcsf error messages
    if (pMessage->ProtId == LCSF_ERROR_PROTOCOL_ID) {
        return LCSF_ProcessReceivedError(pMessage);
    }
    // Retrieve protocol descriptor and callback
    LCSFInterpretCallback *pFnInterpreter = LCSF_GetCallback(pMessage->ProtId);
    const lcsf_protocol_desc_t *pProtDesc = LCSF_GetDescriptor(pMessage->ProtId);
    // Check if protocol id is valid
    if ((pProtDesc == NULL) || (pFnInterpreter == NULL)) {
        return LCSF_ValidatorSendError(LCSF_EP_ERROR_LOC_VALIDATION_ERROR, LCSF_EP_ERROR_CODE_UNKNOWN_PROT_ID);
    }
    // Variables initialization
    uint16_t descCmdIdx = 0;
    lcsf_valid_cmd_t validMsg;
    FiloFreeAll(LcsfValidatorInfo.pReceiverFilo);
    memset(&validMsg, 0, sizeof(lcsf_valid_cmd_t));
    // Check if command id is valid
    if (!LCSF_ValidateCmdId(pMessage->CmdId, pProtDesc->CmdNb, &descCmdIdx, pProtDesc->pCmdDescArray)) {
        return LCSF_ValidatorSendError(LCSF_EP_ERROR_LOC_VALIDATION_ERROR, LCSF_EP_ERROR_CODE_UNKNOWN_CMD_ID);
    }
    // Validate the command id
    validMsg.CmdId = pMessage->CmdId;
    // Check if command attributes are valid
    if (!LCSF_ValidateAttribute(pMessage, &(pProtDesc->pCmdDescArray[descCmdIdx]), &(validMsg.pAttArray))) {
        return LCSF_ValidatorSendError(LCSF_EP_ERROR_LOC_VALIDATION_ERROR, LcsfValidatorInfo.LastErrorCode);
    }
    // Send validated message to interpreter function
    return pFnInterpreter(&validMsg);
}

bool LCSF_ValidatorSend(uint8_t protId, const lcsf_valid_cmd_t *pCommand) {

    if (pCommand == NULL) {
        return false;
    }
    // Retrieve protocol description
    const lcsf_protocol_desc_t *pProtDesc = LCSF_GetDescriptor(protId);
    if (pProtDesc == NULL) {
        return false;
    }
    // Variables initialization
    uint16_t cmdIdx = 0;
    lcsf_raw_msg_t sendMsg;
    memset(&sendMsg, 0, sizeof(lcsf_raw_msg_t));
    FiloFreeAll(LcsfValidatorInfo.pSenderFilo);
    // Note the protocol id
    sendMsg.ProtId = protId;
    // Validate command id
    if (!LCSF_ValidateCmdId(pCommand->CmdId, pProtDesc->CmdNb, &cmdIdx, pProtDesc->pCmdDescArray)) {
        return false;
    }
    // Intermediary variable
    const lcsf_command_desc_t *pCurrCmdDesc = &(pProtDesc->pCmdDescArray[cmdIdx]);
    // Note the command id
    sendMsg.CmdId = pCommand->CmdId;
    // Retrieve attribute number
    sendMsg.AttNb = LCSF_CountAttributes(pCurrCmdDesc->AttNb, pCommand->pAttArray);
    // Validate attribute number
    if (sendMsg.AttNb == 0) {
        if (LCSF_HasNonOptionalAttribute(pCurrCmdDesc->AttNb, pCurrCmdDesc->pAttDescArray)) {
            // Attributes are missing
            return false;
        }
        // No attributes needed, send the message
        return LCSF_TranscoderSend(&sendMsg);
    } else if (sendMsg.AttNb > pCurrCmdDesc->AttNb) {
        // Too many attributes
        return false;
    }
    // Fill message attributes array
    if (!LCSF_FillAttributeArray(sendMsg.AttNb, pCurrCmdDesc->AttNb, pCommand->pAttArray, pCurrCmdDesc, &(sendMsg.pAttArray))) {
        return false;
    }
    // Send message
    return LCSF_TranscoderSend(&sendMsg);
}