/**
 * \file LCSF_Transcoder.c
 * \brief Light Command Set Format transcoder module
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
// Error code enum
enum _lcsf_decode_error_enum {
    LCSF_DECODE_FORMAT_ERROR = 0x00, // Message formatting error, missing or leftover data compared to what's expected
    LCSF_DECODE_OVERFLOW_ERROR = 0x01, // The message is too big/complex to be processed by the module
    LCSF_DECODE_UNKNOWN_ERROR = 0xFF, // Unknown error
};

// Module information structure
typedef struct _lcsf_trnscdr_info {
    uint8_t LastErrCode; // Last error code encountered during decoding
    uint8_t *pEncoderBuffer; // Pointer to the transmission buffer
    filo_desc_t *pDecoderFilo; // Pointer to the decoder filo
    const lcsf_trnscdr_init_desc_t *pInitDesc; // Pointer to module initialization descriptor
    lcsf_raw_msg_t DecoderMsg; // Structure of the decoder message header
} lcsf_trnscdr_info_t;

// --- Private Constants ---
// --- Private Function Prototypes ---
// Decode functions
static bool LCSF_AllocateAttArray(uint16_t attNb, lcsf_raw_att_t **pAttArray);
static bool LCSF_FetchMsgHeader(uint16_t *pBuffIdx, uint16_t buffSize, const uint8_t *pBuffer, lcsf_raw_msg_t *pMsg);
static bool LCSF_FetchAttHeader(uint16_t *pBuffIdx, uint16_t buffSize, const uint8_t *pBuffer, lcsf_raw_att_t *pAtt);
static bool LCSF_FetchAttData(uint16_t *pBuffIdx, uint16_t buffSize, const uint8_t *pBuffer, uint16_t attDataSize, uint8_t **pAttData);
static bool LCSF_DecodeAtt_Rec(uint16_t *pBuffIdx, const uint8_t *pBuffer, uint16_t buffSize, uint16_t attNb, lcsf_raw_att_t *pAttArray);
static bool LCSF_DecodeBuffer(const uint8_t *pBuffer, uint16_t buffSize, lcsf_raw_msg_t *pMsg);
// Encode functions
static bool LCSF_FillMsgHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_msg_t *pMsg);
static bool LCSF_FillAttHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_att_t *pAtt);
static bool LCSF_FillAttData(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_att_t *pAtt);
static bool LCSF_EncodeAtt_Rec(uint16_t *pBuffIdx, uint8_t *pBuffer, uint16_t attNb, const lcsf_raw_att_t *pAttArray);
static bool LCSF_EncodeBuffer(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_msg_t *pMsg);
// Other functions
static bool LCSF_TranscoderCheckInitDesc(const lcsf_trnscdr_init_desc_t *pInitDesc);

// --- Private Variables ---
static lcsf_trnscdr_info_t LcsfTranscoderInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static bool LCSF_AllocateAttArray(uint16_t attNb, lcsf_raw_att_t **pAttArray)
 * \brief Allocate an lcsf attribute array
 *
 * \param attNb number of attributes in the array
 * \param pAttArray pointer to contain the array address
 * \return bool: true if operation was a success
 */
static bool LCSF_AllocateAttArray(uint16_t attNb, lcsf_raw_att_t **pAttArray) {
    // Attempt to allocate the array from the filo
    return FiloGet(LcsfTranscoderInfo.pDecoderFilo, attNb, (void **)pAttArray);
}

/**
 * \fn static bool LCSF_FetchMsgHeader(uint16_t *pBuffIdx, uint16_t buffSize, const uint8_t *pBuffer, lcsf_raw_msg_t *pMsg)
 * \brief Fill an lcsf_msg_header struct from a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param buffSize size of the buffer
 * \param pBuffer pointer to the buffer
 * \param pMsg pointer to the lcsf_msg_header
 * \return bool: true if operation was a success
 */
static bool LCSF_FetchMsgHeader(uint16_t *pBuffIdx, uint16_t buffSize, const uint8_t *pBuffer, lcsf_raw_msg_t *pMsg) {
    // Guard against buffer overflow
    if (*pBuffIdx + 5 < buffSize) {
        // Byte 1: Protocol ID LSB
        pMsg->ProtId = pBuffer[(*pBuffIdx)++];
        // Byte 2: Protocol ID MSB
        pMsg->ProtId += pBuffer[(*pBuffIdx)++] << 8;
        // Byte 3: Command ID LSB
        pMsg->CmdId = pBuffer[(*pBuffIdx)++];
        // Byte 4: Command ID MSB
        pMsg->CmdId += pBuffer[(*pBuffIdx)++] << 8;
        // Byte 5: Attribute Number LSB
        pMsg->AttNb = pBuffer[(*pBuffIdx)++];
        // Byte 6: Attribute Number MSB
        pMsg->AttNb += pBuffer[(*pBuffIdx)++] << 8;
        return true;
    } else {
        return false;
    }
}

/**
 * \fn static bool LCSF_FetchAttHeader(uint16_t *pBuffIdx, uint16_t buffSize, const uint8_t *pBuffer, lcsf_raw_att_t *pAtt)
 * \brief Fill an lcsf_att_header struct from a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param buffSize size of the buffer
 * \param pBuffer pointer to the buffer
 * \param pAtt pointer to the lcsf_msg_header
 * \return bool: true if operation was a success
 */
static bool LCSF_FetchAttHeader(uint16_t *pBuffIdx, uint16_t buffSize, const uint8_t *pBuffer, lcsf_raw_att_t *pAtt) {
    // Guard against buffer overflow
    if (*pBuffIdx + 1 < buffSize) {
        // Byte 1: Attribute id LSB
        pAtt->AttId = pBuffer[(*pBuffIdx)++];
        // Byte 2 (MSbit): Attribute complexity (does it have sub attributes or not)
        pAtt->HasSubAtt = ((pBuffer[*pBuffIdx] & (1 << 7)) != 0);
        // Byte 2: Attribute id MSB
        pAtt->AttId += pBuffer[(*pBuffIdx)++] << 8;
    } else {
        return false;
    }
    // Guard against buffer overflow
    if(*pBuffIdx + 1 < buffSize) {
        // Byte 3: Data size or Sub-attribute number LSB
        pAtt->PayloadSize = pBuffer[(*pBuffIdx)++];
        // Byte 4: Data size or Sub-attribute number MSB
        pAtt->PayloadSize += pBuffer[(*pBuffIdx)++] << 8;
    } else {
        return false;
    }
    return true;
}

/**
 * \fn static bool LCSF_FetchAttData(uint16_t *pBuffIdx, uint16_t buffSize, const uint8_t *pBuffer, uint16_t attDataSize, uint8_t **pAttData)
 * \brief Retrieve the data of an lcsf attribute from a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param buffSize size of the buffer
 * \param pBuffer pointer to the buffer
 * \param attDataSize size of the data to retrieve
 * \param pAtt pointer to contain the data address
 * \return bool: true if operation was a success
 */
static bool LCSF_FetchAttData(uint16_t *pBuffIdx, uint16_t buffSize, const uint8_t *pBuffer, uint16_t attDataSize, uint8_t **pAttData) {
    // Guard against buffer overflow
    if (*pBuffIdx + attDataSize - 1 < buffSize) {
        // We only recopy the pointer, the buffer must not to be re-written before being processed !
        *pAttData = (uint8_t *)&(pBuffer[*pBuffIdx]);
        // We increase the buffer index as we processed the data
        *pBuffIdx += attDataSize;
        return true;
    } else {
        return false;
    }
}

/**
 * \fn static bool LCSF_AttDecode_Rec(uint16_t *pBuffIdx, const uint8_t *pBuffer, uint16_t buffSize, uint16_t attNb, lcsf_raw_att_t *pAttArray)
 * \brief Decode recursively the lcsf attributes from a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param buffSize size of the buffer
 * \param attNb number of attributes in the array
 * \param pAttArray pointer to the attributes array
 * \param pFilo pointer to the decoder filo
 * \return bool: true if operation was a success
 */
static bool LCSF_DecodeAtt_Rec(uint16_t *pBuffIdx, const uint8_t *pBuffer, uint16_t buffSize, uint16_t attNb, lcsf_raw_att_t *pAttArray) {
    // We go through the attribute array
    for (uint16_t attIdx = 0; attIdx < attNb; attIdx++) {
        // Guard against buffer overflow
        if (*pBuffIdx < buffSize) {
            // Decode current attribute header
            if (!LCSF_FetchAttHeader(pBuffIdx, buffSize, pBuffer, &(pAttArray[attIdx]))) {
                LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_FORMAT_ERROR;
                return false;
            }
            // Test if attribute has data or sub-attributes
            if (pAttArray[attIdx].HasSubAtt) {
                // Allocate the sub-attribute array
                if (!LCSF_AllocateAttArray(pAttArray[attIdx].DataSize, &(pAttArray[attIdx].Payload.pSubAttArray))) {
                    LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_OVERFLOW_ERROR;
                    return false;
                }
                // Decode current attribute sub-attribute array
                if (!LCSF_DecodeAtt_Rec(pBuffIdx, pBuffer, buffSize, pAttArray[attIdx].DataSize, pAttArray[attIdx].Payload.pSubAttArray)) {
                    return false;
                }
            } else {
                // Decode attribute data
                if (!LCSF_FetchAttData(pBuffIdx, buffSize, pBuffer, pAttArray[attIdx].DataSize, &(pAttArray[attIdx].Payload.pData))) {
                    LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_FORMAT_ERROR;
                    return false;
                }
            }
        } else {
            LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_FORMAT_ERROR;
            return false;
        }
    }
    return true;
}

/**
 * \fn static bool LCSF_DecodeBuffer(const uint8_t *pBuffer, uint16_t buffSize, lcsf_raw_msg_t *pMsg)
 * \brief Decode a buffer into an lcsf_msg_header
 *
 * \param pBuffer pointer to the buffer
 * \param buffSize size of the buffer
 * \param pMsg pointer to contain the message header
 * \param pFilo pointer to the decoder filo
 * \return bool: true if operation was a success
 */
static bool LCSF_DecodeBuffer(const uint8_t *pBuffer, uint16_t buffSize, lcsf_raw_msg_t *pMsg) {
    uint16_t buffIdx = 0;

    // Clear filo memory
    FiloFreeAll(LcsfTranscoderInfo.pDecoderFilo);
    // Message header initialization
    memset(pMsg, 0, sizeof(lcsf_raw_msg_t));
    // Decode message header
    if (!LCSF_FetchMsgHeader(&buffIdx, buffSize, pBuffer, pMsg)) {
        LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_FORMAT_ERROR;
        return false;
    }
    // Test if there is attributes to decode
    if (pMsg->AttNb > 0) {
        // Allocate the attribute array
        if (!LCSF_AllocateAttArray(pMsg->AttNb, &(pMsg->pAttArray))) {
            LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_OVERFLOW_ERROR;
            return false;
        }
        // Decode attribute array
        if (!LCSF_DecodeAtt_Rec(&buffIdx, pBuffer, buffSize, pMsg->AttNb, pMsg->pAttArray)) {
            return false;
        }
    }
    // There was no error but there is leftover data, this shouldn't happen
    if (buffIdx != buffSize) {
        LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_FORMAT_ERROR;
        return false;
    }
    return true;
}

/**
 * \fn static bool LCSF_FillMsgHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_msg_t *pMsg)
 * \brief Encode an lcsf_msg_header into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param pMsg pointer to the message header
 * \return bool: true if operation was a success
 */
static bool LCSF_FillMsgHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_msg_t *pMsg) {
    // Guard against buffer overflow
    if (*pBuffIdx + 5 < LcsfTranscoderInfo.pInitDesc->BufferSize) {
        // Byte 1 : Protocol id LSB
        pBuffer[(*pBuffIdx)++] = (uint8_t)pMsg->ProtId;
        // Byte 2 : Protocol id MSB
        pBuffer[(*pBuffIdx)++] = (uint8_t)(pMsg->ProtId >> 8);
        // Byte 3 : Command id LSB
        pBuffer[(*pBuffIdx)++] = (uint8_t)pMsg->CmdId;
        // Byte 4 : Command id MSB
        pBuffer[(*pBuffIdx)++] = (uint8_t)(pMsg->CmdId >> 8);
        // Byte 5 : Attribute number LSB
        pBuffer[(*pBuffIdx)++] = (uint8_t)pMsg->AttNb;
        // Byte 6 : Attribute number MSB
        pBuffer[(*pBuffIdx)++] = (uint8_t)(pMsg->AttNb >> 8);
        return true;
    } else {
        return false;
    }
}

/**
 * \fn static bool LCSF_FillAttHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_att_t *pAtt, uint16_t subAttSize)
 * \brief Encode an lcsf_att_header into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param pAtt pointer to the attribute
 * \return bool: true if operation was a success
 */
static bool LCSF_FillAttHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_att_t *pAtt) {

    // Guard against buffer overflow
    if (*pBuffIdx + 3 < LcsfTranscoderInfo.pInitDesc->BufferSize) {
        // Byte 1: Attribute id LSB
        pBuffer[(*pBuffIdx)++] = (uint8_t)pAtt->AttId;
        // Check if attribute has sub attributes
        if (pAtt->HasSubAtt) {
            // Byte 2: Attribute id MSB + MSb at 1
            pBuffer[(*pBuffIdx)++] = (uint8_t)((pAtt->AttId >> 8) | 0x80);
        } else {
            // Byte 2: Attribute id MSB + MSb at 0
            pBuffer[(*pBuffIdx)++] = (uint8_t)((pAtt->AttId >> 8) & 0x7F);
        }
        // Byte 3: Attribute data size or sub-attribute number LSB
        pBuffer[(*pBuffIdx)++] = (uint8_t)pAtt->DataSize;
        // Byte 4: Attribute data size or sub-attribute number MSB
        pBuffer[(*pBuffIdx)++] = (uint8_t)(pAtt->DataSize >> 8);
        return true;
    } else {
        return false;
    }
}

/**
 * \fn static bool LCSF_FillAttData(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_att_t *pAtt)
 * \brief Encode an attribute data into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param pAtt pointer to the attribute
 * \return bool: true if operation was a success
 */
static bool LCSF_FillAttData(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_att_t *pAtt) {
    // Guard against buffer overflow
    if (pAtt->DataSize < LcsfTranscoderInfo.pInitDesc->BufferSize) {
        // Copy data into the buffer
        memcpy(&(pBuffer[*pBuffIdx]), pAtt->Payload.pData, pAtt->DataSize);
        // Increment buffer index
        *pBuffIdx += pAtt->DataSize;
        return true;
    } else {
        return false;
    }
}

/**
 * \fn static int32_t LCSF_AttEncode_Rec(uint16_t *pBuffIdx, uint8_t *pBuffer, uint16_t attNb, const lcsf_raw_att_t *pAttArray)
 * \brief Recursively encode an lcsf_att_header array into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param attNb number of attributes in the array
 * \param pAttArray pointer to the attribute header array
 * \return int32_t: Number of data bytes (sub-attributes included) of an attribute (-1 if error)
 */
static bool LCSF_EncodeAtt_Rec(uint16_t *pBuffIdx, uint8_t *pBuffer, uint16_t attNb, const lcsf_raw_att_t *pAttArray) {
    // We go through the attribute array
    for (uint16_t attIdx = 0; attIdx < attNb; attIdx++) {
        // Guard against buffer overflow
        if (*pBuffIdx >= LcsfTranscoderInfo.pInitDesc->BufferSize) {
            return false;
        }
        // Test if attribute has sub-attributes
        if (pAttArray[attIdx].HasSubAtt) {
            // Fill attribute header (we add the 2 bytes from the sub-attribute number field to the size)
            if (!LCSF_FillAttHeader(pBuffIdx, pBuffer, &(pAttArray[attIdx]))) {
	             return false;
            }
            // Encode current attribute sub-attribute array
            if (!LCSF_EncodeAtt_Rec(pBuffIdx, pBuffer, pAttArray[attIdx].DataSize, pAttArray[attIdx].Payload.pSubAttArray)) {
	             return false;
	         }
        } else {
            // Fill attribute header
            if (!LCSF_FillAttHeader(pBuffIdx, pBuffer, &(pAttArray[attIdx]))) {
                return false;
            }
            // Copy data into the buffer
            if (!LCSF_FillAttData(pBuffIdx, pBuffer, &(pAttArray[attIdx]))) {
                return false;
            }
        }
    }
    return true;
}

/**
 * \fn static bool LCSF_BufferEncode(uint16_t *pBuffIdx, uint8_t *pBuffer, uint16_t attNb, const lcsf_raw_att_t *pAttArray)
 * \brief Encode lcsf_msg_header into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param pMsg pointer to the message header
 * \return bool: true if operation was a success
 */
static bool LCSF_EncodeBuffer(uint16_t *pBuffIdx, uint8_t *pBuffer, const lcsf_raw_msg_t *pMsg) {
    // Buffer initialization
    memset(pBuffer, 0, LcsfTranscoderInfo.pInitDesc->BufferSize * sizeof(uint8_t));
    // Encode the message header
    if (!LCSF_FillMsgHeader(pBuffIdx, pBuffer, pMsg)) {
        return false;
    }
    // Encode the attribute array
    if (!LCSF_EncodeAtt_Rec(pBuffIdx, pBuffer, pMsg->AttNb, pMsg->pAttArray)) {
        return false;
    }
    return true;
}

/**
 * \fn static bool LCSF_CheckInitDesc(const lcsf_trnscdr_init_desc_t *pInitDesc)
 * \brief Check the validity of the initialization descriptor
 *
 * \param pInitDesc Pointer to the module initialization descriptor
 * \return bool: true if descriptor is valid
 */
static bool LCSF_TranscoderCheckInitDesc(const lcsf_trnscdr_init_desc_t *pInitDesc) {
    if (pInitDesc != NULL) {
        return (pInitDesc->pFnSendMsg != NULL);
    } else {
        return false;
    }
}



// *** Public Functions ***

bool LCSF_TranscoderInit(const lcsf_trnscdr_init_desc_t *pInitDesc) {

    if (LCSF_TranscoderCheckInitDesc(pInitDesc)) {
        // Buffer allocation
        LcsfTranscoderInfo.pEncoderBuffer = MemAllocMalloc(pInitDesc->BufferSize);
        // Filo creation
        LcsfTranscoderInfo.pDecoderFilo = FiloCreate(pInitDesc->FiloSize, sizeof(lcsf_raw_att_t));
        // Structure initialization
        memset(&LcsfTranscoderInfo.DecoderMsg, 0, sizeof(lcsf_raw_msg_t));
        // Variables initialization
        LcsfTranscoderInfo.LastErrCode = 0;
        LcsfTranscoderInfo.pInitDesc = pInitDesc;
        return true;
    } else {
        return false;
    }
}

bool LCSF_TranscoderReceive(const uint8_t *pBuffer, uint16_t buffSize) {

    if (pBuffer != NULL) {
        lcsf_raw_msg_t *pMsg = &LcsfTranscoderInfo.DecoderMsg;
        // Decode buffer into lcsf object
        if(LCSF_DecodeBuffer(pBuffer, buffSize, pMsg)) {
            // Send lcsf object to receiver
            return LCSF_ValidatorReceive(pMsg);
        }
    } else {
        LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_UNKNOWN_ERROR;
    }
    // Send error if decode unsuccessful
    return LCSF_ValidatorSendTranscoderError(LcsfTranscoderInfo.LastErrCode);
}

bool LCSF_TranscoderSend(const lcsf_raw_msg_t *pMessage) {

    if (pMessage != NULL) {
        uint16_t buffSize = 0;
        uint8_t *pBuffer = LcsfTranscoderInfo.pEncoderBuffer;
        // Encode lcsf object into buffer
        if (LCSF_EncodeBuffer(&buffSize, pBuffer, pMessage)) {
            // Send buffer to transmitter
            return LcsfTranscoderInfo.pInitDesc->pFnSendMsg(pBuffer, buffSize);
        }
    }
    return false;
}