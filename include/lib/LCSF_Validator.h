/**
 * \file LCSF_Validator.h
 * \brief Light Command Set Format validator module
 * \author Jean-Roland Gosse
 *
 * This file is part of LCSF C Stack.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>
 */

#ifndef _LCSF_Validator_h
#define _LCSF_Validator_h

// *** Libraries include ***
// Standard lib
// Custom lib
#include <LCSF_Config.h>
#include <lib/LCSF_Transcoder.h>

// *** Definitions ***
// --- Public Types ---

/** Data type enum */
enum _lcsf_data_type {
    LCSF_UINT8,
    LCSF_UINT16,
    LCSF_UINT32,
    LCSF_UINT64,
    LCSF_FLOAT32,
    LCSF_FLOAT64,
    LCSF_BYTE_ARRAY,
    LCSF_STRING,
    LCSF_SUB_ATTRIBUTES,
};

// Lcsf descriptor types

// Forward declaration to resolve self-referencing
typedef struct _lcsf_attribute_desc lcsf_attribute_desc_t;

/** Lcsf attribute descriptor structure */
struct _lcsf_attribute_desc {
    bool IsOptional; /**< Indicates if the attribute is optional or not */
    uint8_t DataType; /**< Attribute data type */
    uint16_t AttId; /**< Attribute identifier */
    uint16_t SubAttNb; /**< Number of possible sub attributes */
    const lcsf_attribute_desc_t *pSubAttDescArray; /**< Pointer to the sub attribute descriptor table */
}; // total: 12 bytes, 2 padding

/** Lcsf command descriptor structure */
typedef struct _lcsf_command_desc {
    uint16_t CmdId; /**< Command identifier */
    uint16_t AttNb; /**< Number of possible attributes */
    const lcsf_attribute_desc_t *pAttDescArray; /**< Pointer to the attribute descriptor table */
} lcsf_command_desc_t; // total: 8 bytes, 0 padding

/** Lcsf protocol descriptor structure */
typedef struct _lcsf_protocol_desc {
    uint16_t CmdNb; /**< Number of possible commands */
    const lcsf_command_desc_t *pCmdDescArray; /**< Pointer to the command descriptor table */
} lcsf_protocol_desc_t; // total: 8 bytes, 2 padding

// Lcsf valid types

// Forward declaration to resolve cross-referencing
typedef struct _lcsf_valid_att lcsf_valid_att_t;

/** Lcsf valid attribute payload union */
typedef union _lcsf_valid_att_payload {
    void *pData; /**< Pointer to data */
    lcsf_valid_att_t *pSubAttArray; /**< Pointer to sub-attribute array */
} lcsf_valid_att_payload_t; // total: 4 bytes, 0 padding

/** Lcsf valid attribute structure */
struct _lcsf_valid_att {
    uint32_t PayloadSize; /**< Size of the payload */
    lcsf_valid_att_payload_t Payload; /**< Attribute payload */
}; // total: 8 bytes, 0 padding

/** Lcsf valid command structure */
typedef struct _lcsf_valid_cmd {
    uint16_t CmdId; /**< Command identifier */
    lcsf_valid_att_t *pAttArray; /**< Pointer to attribute array */
} lcsf_valid_cmd_t; // total: 8 bytes, 2 padding

// Lcsf validator module types

// Callback prototypes
typedef bool(LCSFInterpretCallback_t)(lcsf_valid_cmd_t *pValidCmd);
typedef bool(LCSFSendErrCallback_t)(const uint8_t *pBuffer, size_t buffSize);
typedef void(LCSFReceiveErrCallback_t)(uint_fast8_t errorLoc, uint_fast8_t errorType);

/** Protocol description structure */
typedef struct _lcsf_validator_protocol_desc {
    uint16_t ProtId; /**< Protocol identifier */
    const lcsf_protocol_desc_t *pProtDesc; /**< Pointer to protocol descriptor */
    LCSFInterpretCallback_t *pFnInterpretMsg; /**< Pointer to the message interpretation function */
} lcsf_validator_protocol_desc_t; // total: 12 bytes, 2 padding

// Functions used by transcoder to send errors
bool LCSF_ValidatorSendTranscoderError(uint_fast8_t errorType);

// --- Public Constants ---
// --- Public Variables ---
// --- Public Function Prototypes ---

/**
 * \fn bool LCSF_ValidatorInit(LCSFSendErrCallback_t *pFnSendErrCb, LCSFReceiveErrCallback_t *pFnRecErrCb)
 * \brief Initialize the module
 *
 * \param pFnSendErrCb Function pointer to send lcsf error messages (optional)
 * \param pFnSendErrCb Function pointer to receive lcsf error messages (optional)
 * \return bool: true if operation was a success
 */
bool LCSF_ValidatorInit(LCSFSendErrCallback_t *pFnSendErrCb, LCSFReceiveErrCallback_t *pFnRecErrCb);

/**
 * \fn bool LCSF_ValidatorAddProtocol(uint_fast16_t protId, const lcsf_validator_protocol_desc_t *pProtDesc)
 * \brief Add a protocol to the module table
 *
 * \param protIdx protocol index in the module table
 * \param pProtDesc pointer to protocol descriptor
 * \return bool: true if operation was a success
 */
bool LCSF_ValidatorAddProtocol(uint_fast16_t protIdx, const lcsf_validator_protocol_desc_t *pProtDesc);

/**
 * \fn bool LCSF_ValidatorReceive(const lcsf_raw_msg_t *pMessage)
 * \brief Validate a raw lcsf message and send to interpreter
 *
 * \param pMessage Pointer to message to validate
 * \return bool: true if operation was a success
 */
bool LCSF_ValidatorReceive(const lcsf_raw_msg_t *pMessage);

/**
 * \fn int LCSF_ValidatorEncode(uint_fast16_t protId, lcsf_valid_cmd_t *pCommand)
 * \brief Send a valid command to lcsf transcoder for encoding
 *
 * \param protId protocol identifier
 * \param pCommand Pointer to the command to send
 * \param pBuffer pointer to the send buffer
 * \param buffSize buffer size
 * \return int: -1 if operation failed, encoded message size if success
 */
int LCSF_ValidatorEncode(uint_fast16_t protId, const lcsf_valid_cmd_t *pCommand, uint8_t *pBuffer, size_t buffSize);

// *** End Definitions ***
#endif // _LCSF_Interpreter_h