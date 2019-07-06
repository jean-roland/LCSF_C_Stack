/**
 * \file LCSF_Transcoder.h
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

#ifndef _LCSF_Transcoder_h
#define _LCSF_Transcoder_h

// *** Libraries include ***
// Standard lib
#include <stdbool.h>
#include <stdint.h>
// Custom lib

// *** Definitions ***
// --- Public Types ---
// Forward declaration to resolve cross referencing
typedef struct _lcsf_raw_att lcsf_raw_att_t;

typedef union _lcsf_raw_att_payload {
    uint8_t *pData; // Pointer to data
    lcsf_raw_att_t *pSubAttArray; // Pointer to lcsf sub attribute header list
} lcsf_raw_att_payload_t; // total : 4 bytes, 0 padding

struct _lcsf_raw_att {
    uint16_t AttId: 15; // Lcsf attribute id
    uint16_t HasSubAtt: 1; // Indicates if the attribute has sub attributes or data
    uint16_t DataSize; // Data size (bytes) or lcsf sub attribute number
    lcsf_raw_att_payload_t Payload; // See lcsf_att_field_t
}; // total : 8 bytes, 0 padding

typedef struct _lcsf_raw_msg {
    uint16_t ProtId; // Protocol id
    uint16_t CmdId; // Command id
    uint16_t AttNb; // Number of attributes
    lcsf_raw_att_t *pAttArray; // Pointer to the lcsf attribute header list
} lcsf_raw_msg_t; // total : 12 bytes, 2 padding

// Callback prototype
typedef bool LCSFSendCallback(const uint8_t *pBuffer, uint16_t buffSize);

// Module initialization descriptor structure
typedef struct _lcsf_trnscdr_init_desc {
    LCSFSendCallback *pFnSendMsg; // Used to send encoded lcsf messages
    uint16_t FiloSize; // Size of the filo used by the module (number of element)
    uint16_t BufferSize; // Size of the module encoder buffer (byte)
} lcsf_trnscdr_init_desc_t;

// --- Public Constants ---
// --- Public Variables ---
// --- Public Function Prototypes ---

/**
 * \fn bool LCSF_TranscoderInit(const lcsf_trnscdr_init_desc_t *pInitDesc)
 * \brief Initialize the module
 *
 * \param pInitDesc pointer to module initialization descriptor
 * \return bool: true if operation was a success
 */
bool LCSF_TranscoderInit(const lcsf_trnscdr_init_desc_t *pInitDesc);

/**
 * \fn bool LCSF_TranscoderReceive(const uint8_t *pBuffer, uint16_t buffSize)
 * \brief Decode a buffer into a raw lcsf message and pass it to receiver
 *
 * \param pBuffer pointer to the data to decode
 * \param buffSize buffer size
 * \return bool: true if operation was a success
 */
bool LCSF_TranscoderReceive(const uint8_t *pBuffer, uint16_t buffSize);

/**
 * \fn bool LCSF_TranscoderSend(const lcsf_raw_msg_t *pMessage)
 * \brief Encode a raw lcsf message into a buffer and pass it to sender
 *
 * \param pMessage pointer to the raw lcsf message to encode
 * \return bool: true if operation was a success
 */
bool LCSF_TranscoderSend(const lcsf_raw_msg_t *pMessage);

// *** End Definitions ***
#endif // _LCSF_Transcoder_h