/**
 * \file LCSF_Validator.h
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

#ifndef _LCSF_Validator_h
#define _LCSF_Validator_h

// *** Libraries include ***
// Standard lib
#include <stdbool.h>
#include <stdint.h>
// Custom lib
#include <LCSF_Transcoder.h>

// *** Definitions ***
// --- Public Types ---

// Data type enum
enum _lcsf_data_type {
    LCSF_UINT8,
    LCSF_UINT16,
    LCSF_UINT32,
    LCSF_BYTE_ARRAY,
    LCSF_STRING,
    LCSF_SUB_ATTRIBUTES,
};

// Lcsf descriptor types

// Forward declaration to resolve self-referencing
typedef struct _lcsf_attribute_desc lcsf_attribute_desc_t;

// Lcsf attribute descriptor structure
struct _lcsf_attribute_desc {
    bool IsOptional; // Indicates if the attribute is optional or not
    uint8_t DataType; // Attribute data type
    uint16_t AttId; // Attribute identifier
    uint16_t SubAttNb; // Number of possible sub attributes
    const lcsf_attribute_desc_t *pSubAttDescArray; // Pointer to the sub attribute descriptor table
}; // total : 12 bytes, 2 padding

// Lcsf command descriptor structure
typedef struct _lcsf_command_desc {
    uint16_t CmdId; // Command identifier
    uint16_t AttNb; // Number of possible attributes
    const lcsf_attribute_desc_t *pAttDescArray; // Pointer to the attribute descriptor table
} lcsf_command_desc_t; // total : 8 bytes, 0 padding

// Lcsf protocol descriptor structure
typedef struct _lcsf_protocol_desc {
    uint16_t CmdNb; // Number of possible commands
    const lcsf_command_desc_t *pCmdDescArray; // Pointer to the command descriptor table
} lcsf_protocol_desc_t; // total : 8 bytes, 2 padding

// Lcsf valid types

// Forward declaration to resolve cross-referencing
typedef struct _lcsf_valid_att lcsf_valid_att_t;

typedef union _lcsf_valid_att_payload {
    void *pData; // Pointer to data
    lcsf_valid_att_t *pSubAttArray; // Pointer to sub-attribute array
} lcsf_valid_att_payload_t; // total : 4 bytes, 0 padding

struct _lcsf_valid_att {
    uint32_t PayloadSize; // Size of the payload
    lcsf_valid_att_payload_t Payload; // Attribute payload
}; // total : 8 bytes, 0 padding

typedef struct _lcsf_valid_cmd {
    uint16_t CmdId;  // Command identifier
    lcsf_valid_att_t *pAttArray; // Pointer to attribute array
} lcsf_valid_cmd_t; // total : 8 bytes, 2 padding

// Lcsf validator module types

// Callback prototype
typedef bool LCSFInterpretCallback(lcsf_valid_cmd_t *pValidCmd);

// Protocol description structure
typedef struct _lcsf_validator_protocol_desc {
    uint8_t ProtId; // Protocol identifier
    const lcsf_protocol_desc_t *pProtDesc; // Pointer to protocol descriptor
    LCSFInterpretCallback *pFnInterpretMsg; // Pointer to the message interpretation function
} lcsf_validator_protocol_desc_t;

// --- Public Constants ---
// --- Public Variables ---
// --- Public Function Prototypes ---

/**
 * \fn bool LCSF_ValidatorInit(const lcsf_validator_init_desc_t *pInitDesc)
 * \brief Initialize the module
 *
 * \param protNb Number of protocols the module will handle
 * \return bool: true if operation was a success
 */
bool LCSF_ValidatorInit(uint8_t protNb);

/**
 * \fn bool LCSF_ValidatorAddProtocol(uint8_t protId, const lcsf_validator_protocol_desc_t *pProtDesc)
 * \brief Add a protocol to the module table
 *
 * \param protIdx protocol index in the module table
 * \param pProtDesc pointer to protocol descriptor
 * \return bool: true if operation was a success
 */
bool LCSF_ValidatorAddProtocol(uint8_t protIdx, const lcsf_validator_protocol_desc_t *pProtDesc);

/**
 * \fn bool LCSF_ValidatorSendTranscoderError(uint8_t errorType)
 * \brief Send an LCSF transcoder error message
 *
 * \param errorType error code to send
 * \return bool: true if operation was a success
 */
bool LCSF_ValidatorSendTranscoderError(uint8_t errorType);

/**
 * \fn bool LCSF_ValidatorTakeReceivedError(uint8_t *pErrLoc, uint8_t *pErrType)
 * \brief Retrieve the last received lcsf error
 *
 * \param pErrLoc pointer to contain error location
 * \param pErrType pointer to contain error type
 * \return bool: true if error available and successfully copied to arguments
 */
bool LCSF_ValidatorTakeReceivedError(uint8_t *pErrLoc, uint8_t *pErrType);

/**
 * \fn bool LCSF_ValidatorReceive(const lcsf_raw_msg_t *pMessage)
 * \brief Validate a raw lcsf message and send to interpreter
 *
 * \param pMessage Pointer to message to validate
 * \return bool: true if operation was a success
 */
bool LCSF_ValidatorReceive(const lcsf_raw_msg_t *pMessage);

/**
 * \fn bool LCSF_ValidatorSend(uint8_t protId, lcsf_valid_cmd_t *pCommand)
 * \brief Send a valid command to lcsf transcoder
 *
 * \param protId protocol identifier
 * \param pCommand Pointer to the command to send
 * \return bool: true if operation was a success
 */
bool LCSF_ValidatorSend(uint8_t protId, const lcsf_valid_cmd_t *pCommand);

// *** End Definitions ***
#endif // _LCSF_Interpreter_h