/**
 * \file LCSF_Transcoder.h
 * \brief Light Command Set Format transcoder module
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

#ifndef _LCSF_Transcoder_h
#define _LCSF_Transcoder_h

// *** Libraries include ***
// Standard lib
// Custom lib
#include <LCSF_Config.h>

// *** Definitions ***
// --- Public Types ---
// Forward declaration to resolve cross referencing
typedef struct _lcsf_raw_att lcsf_raw_att_t;

/** Lcsf raw attribute payload union */
typedef union _lcsf_raw_att_payload {
    uint8_t *pData; /**< Pointer to data */
    lcsf_raw_att_t *pSubAttArray; /**< Pointer to lcsf sub attribute header list */
} lcsf_raw_att_payload_t; // total: 4 bytes, 0 padding

/** Lcsf raw attribute structure */
struct _lcsf_raw_att {
    uint16_t AttId:15; /**< Lcsf attribute id */
    uint16_t HasSubAtt:1; /**< Indicates if the attribute has sub attributes or data */
    uint16_t PayloadSize; /**< Data size (bytes) or lcsf sub attribute number */
    lcsf_raw_att_payload_t Payload; // See lcsf_att_field_t
}; // total: 8 bytes, 0 padding

/** Lcsf raw message structure */
typedef struct _lcsf_raw_msg {
    uint16_t ProtId; /**< Protocol id */
    uint16_t CmdId; /**< Command id */
    uint16_t AttNb; /**< Number of attributes */
    lcsf_raw_att_t *pAttArray; /**< Pointer to the lcsf attribute header list */
} lcsf_raw_msg_t; // total: 12 bytes, 2 padding

// --- Public Constants ---
// --- Public Variables ---
// --- Public Function Prototypes ---

/**
 * \fn bool LCSF_TranscoderInit(LCSFSendCallback *pFnSendMsg)
 * \brief Initialize the module
 *
 * \return bool: true if operation was a success
 */
bool LCSF_TranscoderInit(void);

/**
 * \fn bool LCSF_TranscoderReceive(const uint8_t *pBuffer, size_t buffSize)
 * \brief Decode a buffer into a raw lcsf message and pass it to receiver
 *
 * \param pBuffer pointer to the data to decode
 * \param buffSize buffer size
 * \return bool: true if operation was a success
 */
bool LCSF_TranscoderReceive(const uint8_t *pBuffer, size_t buffSize);

/**
 * \fn int LCSF_TranscoderEncode(const lcsf_raw_msg_t *pMessage, uint8_t* pBuffer, size_t buffSize)
 * \brief Encode a raw lcsf message into a buffer
 *
 * \param pMessage pointer to the raw lcsf message to encode
 * \param pBuffer pointer to the send buffer
 * \param buffSize buffer size
 * \return int: -1 if operation fail, encoded message size if success
 */
int LCSF_TranscoderEncode(const lcsf_raw_msg_t *pMessage, uint8_t *pBuffer, size_t buffSize);

// *** End Definitions ***
#endif // _LCSF_Transcoder_h