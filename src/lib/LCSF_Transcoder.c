/**
 * \file LCSF_Transcoder.c
 * \brief Light Command Set Format transcoder module
 * \author Jean-Roland Gosse
 *
 * This file is part of LCSF C Stack.
 *
 * LCSF C Stack is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCSF C Stack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this project. If not, see <https://www.gnu.org/licenses/>
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include <Filo.h>
#include <LCSF_Config.h>
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
    uint8_t DecoderFiloData[LCSF_TRANSCODER_RX_FILO_SIZE * sizeof(lcsf_raw_att_t)]; // Decoder filo data buffer
    filo_desc_t DecoderFilo; // Structure of the decoder filo
    lcsf_raw_msg_t DecoderMsg; // Structure of the decoder message header
} lcsf_trnscdr_info_t;

// --- Private Constants ---
// --- Private Function Prototypes ---
// Decode functions
static bool LCSF_AllocateAttArray(uint_fast16_t attNb, lcsf_raw_att_t **pAttArray);
static bool LCSF_FetchMsgHeader(uint16_t *pBuffIdx, size_t buffSize, const uint8_t *pBuffer, lcsf_raw_msg_t *pMsg);
static bool LCSF_FetchAttHeader(uint16_t *pBuffIdx, size_t buffSize, const uint8_t *pBuffer, lcsf_raw_att_t *pAtt);
static bool LCSF_FetchAttData(uint16_t *pBuffIdx, size_t buffSize, const uint8_t *pBuffer, size_t attDataSize, uint8_t **pAttData);
static bool LCSF_DecodeAtt_Rec(uint16_t *pBuffIdx, const uint8_t *pBuffer, size_t buffSize, uint_fast16_t attNb, lcsf_raw_att_t *pAttArray);
static bool LCSF_DecodeBuffer(const uint8_t *pBuffer, size_t buffSize, lcsf_raw_msg_t *pMsg);
// Encode functions
static bool LCSF_FillMsgHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_msg_t *pMsg);
static bool LCSF_FillAttHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_att_t *pAtt);
static bool LCSF_FillAttData(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_att_t *pAtt);
static bool LCSF_EncodeAtt_Rec(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, uint_fast16_t attNb, const lcsf_raw_att_t *pAttArray);
static bool LCSF_EncodeBuffer(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_msg_t *pMsg);
// External functions
extern bool LCSF_ValidatorSendTranscoderError(uint_fast8_t errorType);

// --- Private Variables ---
static lcsf_trnscdr_info_t LcsfTranscoderInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static bool LCSF_AllocateAttArray(uint_fast16_t attNb, lcsf_raw_att_t **pAttArray)
 * \brief Allocate an lcsf attribute array
 *
 * \param attNb number of attributes in the array
 * \param pAttArray pointer to contain the array address
 * \return bool: true if operation was a success
 */
static bool LCSF_AllocateAttArray(uint_fast16_t attNb, lcsf_raw_att_t **pAttArray) {
    // Attempt to allocate the array from the filo
    return FiloGet(&LcsfTranscoderInfo.DecoderFilo, attNb, (void **)pAttArray);
}

/**
 * \fn static bool LCSF_FetchMsgHeader(uint16_t *pBuffIdx, size_t buffSize, const uint8_t *pBuffer, lcsf_raw_msg_t *pMsg)
 * \brief Fill an lcsf_msg_header struct from a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param buffSize size of the buffer
 * \param pBuffer pointer to the buffer
 * \param pMsg pointer to the lcsf_msg_header
 * \return bool: true if operation was a success
 */
static bool LCSF_FetchMsgHeader(uint16_t *pBuffIdx, size_t buffSize, const uint8_t *pBuffer, lcsf_raw_msg_t *pMsg) {
#ifdef LCSF_SMALL
    // Buffer overflow guard
    if ((size_t)(*pBuffIdx + 2) >= buffSize) {
        return false;
    }
    // Byte 1: Protocol id
    pMsg->ProtId = pBuffer[(*pBuffIdx)++];
    // Byte 2: Command id
    pMsg->CmdId = pBuffer[(*pBuffIdx)++];
    // Byte 3: Attribute number
    pMsg->AttNb = pBuffer[(*pBuffIdx)++];
#else
    // Buffer overflow guard
    if ((size_t)(*pBuffIdx + 5) >= buffSize) {
        return false;
    }
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
#endif
    return true;
}

/**
 * \fn static bool LCSF_FetchAttHeader(uint16_t *pBuffIdx, size_t buffSize, const uint8_t *pBuffer, lcsf_raw_att_t *pAtt)
 * \brief Fill an lcsf_att_header struct from a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param buffSize size of the buffer
 * \param pBuffer pointer to the buffer
 * \param pAtt pointer to the lcsf_msg_header
 * \return bool: true if operation was a success
 */
static bool LCSF_FetchAttHeader(uint16_t *pBuffIdx, size_t buffSize, const uint8_t *pBuffer, lcsf_raw_att_t *pAtt) {
#ifdef LCSF_SMALL
    // Buffer overflow guard
    if ((size_t)(*pBuffIdx + 1) >= buffSize) {
        return false;
    }
    // Byte 1 (MSbit): Complexity flag (the attribute have sub-attributes if set)
    pAtt->HasSubAtt = ((pBuffer[*pBuffIdx] & (1 << 7)) != 0);
    // Byte 1: Attribute id
    pAtt->AttId = pBuffer[(*pBuffIdx)++];
    // Mask the complexity flag from attribute id
    pAtt->AttId &= ~(1 << 7);
    // Byte 2: Payload size
    pAtt->PayloadSize = pBuffer[(*pBuffIdx)++];
#else
    // Buffer overflow guard
    if ((size_t)(*pBuffIdx + 3) >= buffSize) {
        return false;
    }
    // Byte 1: Attribute id LSB
    pAtt->AttId = pBuffer[(*pBuffIdx)++];
    // Byte 2 (MSbit): Complexity flag (the attribute have sub-attributes if set)
    pAtt->HasSubAtt = ((pBuffer[*pBuffIdx] & (1 << 7)) != 0);
    // Byte 2: Attribute id MSB
    pAtt->AttId += pBuffer[(*pBuffIdx)++] << 8;
    // Byte 3: Data size or Sub-attribute number LSB
    pAtt->PayloadSize = pBuffer[(*pBuffIdx)++];
    // Byte 4: Data size or Sub-attribute number MSB
    pAtt->PayloadSize += pBuffer[(*pBuffIdx)++] << 8;
#endif
    return true;
}

/**
 * \fn static bool LCSF_FetchAttData(uint16_t *pBuffIdx, size_t buffSize, const uint8_t *pBuffer, size_t attDataSize, uint8_t **pAttData)
 * \brief Retrieve the data of an lcsf attribute from a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param buffSize size of the buffer
 * \param pBuffer pointer to the buffer
 * \param attDataSize size of the data to retrieve
 * \param pAttData pointer to contain the data address
 * \return bool: true if operation was a success
 */
static bool LCSF_FetchAttData(uint16_t *pBuffIdx, size_t buffSize, const uint8_t *pBuffer, size_t attDataSize, uint8_t **pAttData) {
    // Buffer overflow guard
    if ((size_t)(*pBuffIdx + attDataSize) > buffSize) {
        return false;
    }
    // Recopy the pointer, the buffer must not to be re-written before being processed!
    *pAttData = (uint8_t *)&(pBuffer[*pBuffIdx]);
    // Increase the buffer index as data is processed
    *pBuffIdx += attDataSize;
    return true;
}

/**
 * \fn static bool LCSF_AttDecode_Rec(uint16_t *pBuffIdx, const uint8_t *pBuffer, size_t buffSize, uint_fast16_t attNb, lcsf_raw_att_t *pAttArray)
 * \brief Decode recursively the lcsf attributes from a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param buffSize size of the buffer
 * \param attNb number of attributes in the array
 * \param pAttArray pointer to the attributes array
 * \return bool: true if operation was a success
 */
static bool LCSF_DecodeAtt_Rec(uint16_t *pBuffIdx, const uint8_t *pBuffer, size_t buffSize, uint_fast16_t attNb, lcsf_raw_att_t *pAttArray) {
    // Parse through the attribute array
    for (uint16_t attIdx = 0; attIdx < attNb; attIdx++) {
        // Guard against buffer overflow
        if ((size_t)(*pBuffIdx) < buffSize) {
            // Decode current attribute header
            if (!LCSF_FetchAttHeader(pBuffIdx, buffSize, pBuffer, &(pAttArray[attIdx]))) {
                LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_FORMAT_ERROR;
                return false;
            }
            // Test if attribute has data or sub-attributes
            if (pAttArray[attIdx].HasSubAtt) {
                // Allocate the sub-attribute array
                if (!LCSF_AllocateAttArray(pAttArray[attIdx].PayloadSize, &(pAttArray[attIdx].Payload.pSubAttArray))) {
                    LcsfTranscoderInfo.LastErrCode = LCSF_DECODE_OVERFLOW_ERROR;
                    return false;
                }
                // Decode current attribute sub-attribute array
                if (!LCSF_DecodeAtt_Rec(pBuffIdx, pBuffer, buffSize, pAttArray[attIdx].PayloadSize, pAttArray[attIdx].Payload.pSubAttArray)) {
                    return false;
                }
            } else {
                // Decode attribute data
                if (!LCSF_FetchAttData(pBuffIdx, buffSize, pBuffer, pAttArray[attIdx].PayloadSize, &(pAttArray[attIdx].Payload.pData))) {
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
 * \fn static bool LCSF_DecodeBuffer(const uint8_t *pBuffer, size_t buffSize, lcsf_raw_msg_t *pMsg)
 * \brief Decode a buffer into an lcsf_msg_header
 *
 * \param pBuffer pointer to the buffer
 * \param buffSize size of the buffer
 * \param pMsg pointer to contain the message header
 * \param pFilo pointer to the decoder filo
 * \return bool: true if operation was a success
 */
static bool LCSF_DecodeBuffer(const uint8_t *pBuffer, size_t buffSize, lcsf_raw_msg_t *pMsg) {
    uint16_t buffIdx = 0;

    // Clear filo memory
    FiloFreeAll(&LcsfTranscoderInfo.DecoderFilo);
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
 * \fn static bool LCSF_FillMsgHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_msg_t *pMsg)
 * \brief Encode an lcsf_msg_header into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param buffSize buffer size
 * \param pMsg pointer to the message header
 * \return bool: true if operation was a success
 */
static bool LCSF_FillMsgHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_msg_t *pMsg) {
#ifdef LCSF_SMALL
    // Buffer overflow guard
    if ((size_t)(*pBuffIdx + 2) >= buffSize) {
        return false;
    }
    // Byte 1: Protocol id
    pBuffer[(*pBuffIdx)++] = (uint8_t)pMsg->ProtId;
    // Byte 2: Command id
    pBuffer[(*pBuffIdx)++] = (uint8_t)pMsg->CmdId;
    // Byte 3: Attribute number
    pBuffer[(*pBuffIdx)++] = (uint8_t)pMsg->AttNb;
#else
    // Buffer overflow guard
    if ((size_t)(*pBuffIdx + 5) >= buffSize) {
        return false;
    }
    // Byte 1: Protocol id LSB
    pBuffer[(*pBuffIdx)++] = (uint8_t)pMsg->ProtId;
    // Byte 2: Protocol id MSB
    pBuffer[(*pBuffIdx)++] = (uint8_t)(pMsg->ProtId >> 8);
    // Byte 3: Command id LSB
    pBuffer[(*pBuffIdx)++] = (uint8_t)pMsg->CmdId;
    // Byte 4: Command id MSB
    pBuffer[(*pBuffIdx)++] = (uint8_t)(pMsg->CmdId >> 8);
    // Byte 5: Attribute number LSB
    pBuffer[(*pBuffIdx)++] = (uint8_t)pMsg->AttNb;
    // Byte 6: Attribute number MSB
    pBuffer[(*pBuffIdx)++] = (uint8_t)(pMsg->AttNb >> 8);
#endif
    return true;
}

/**
 * \fn static bool LCSF_FillAttHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_att_t *pAtt)
 * \brief Encode an lcsf_att_header into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param buffSize buffer size
 * \param pAtt pointer to the attribute
 * \return bool: true if operation was a success
 */
static bool LCSF_FillAttHeader(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_att_t *pAtt) {
#ifdef LCSF_SMALL
    // Buffer overflow guard
    if ((size_t)(*pBuffIdx + 1) >= buffSize) {
        return false;
    }
    // Check if attribute has sub attributes
    if (pAtt->HasSubAtt) {
        // Byte 1: Attribute id + MSb at 1
        pBuffer[(*pBuffIdx)++] = (uint8_t)((pAtt->AttId) | 0x80);
    } else {
        // Byte 1: Attribute id + MSb at 0
        pBuffer[(*pBuffIdx)++] = (uint8_t)((pAtt->AttId) & 0x7F);
    }
    // Byte 2: Attribute data size or sub-attribute number
    pBuffer[(*pBuffIdx)++] = (uint8_t)pAtt->PayloadSize;
#else
    if ((size_t)(*pBuffIdx + 3) >= buffSize) {
        return false;
    }
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
    pBuffer[(*pBuffIdx)++] = (uint8_t)pAtt->PayloadSize;
    // Byte 4: Attribute data size or sub-attribute number MSB
    pBuffer[(*pBuffIdx)++] = (uint8_t)(pAtt->PayloadSize >> 8);
#endif
    return true;
}

/**
 * \fn static bool LCSF_FillAttData(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_att_t *pAtt)
 * \brief Encode an attribute data into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param buffSize buffer size
 * \param pAtt pointer to the attribute
 * \return bool: true if operation was a success
 */
static bool LCSF_FillAttData(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_att_t *pAtt) {
    // Guard against buffer overflow
    if ((size_t)(*pBuffIdx + pAtt->PayloadSize) > buffSize) {
        return false;
    }
    // Copy data into the buffer
    memcpy(&(pBuffer[*pBuffIdx]), pAtt->Payload.pData, pAtt->PayloadSize);
    // Increment buffer index
    *pBuffIdx += pAtt->PayloadSize;
    return true;
}

/**
 * \fn static int32_t LCSF_AttEncode_Rec(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, uint_fast16_t attNb, const lcsf_raw_att_t *pAttArray)
 * \brief Recursively encode an lcsf_att_header array into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param buffSize buffer size
 * \param attNb number of attributes in the array
 * \param pAttArray pointer to the attribute header array
 * \return int32_t: Number of data bytes (sub-attributes included) of an attribute (-1 if error)
 */
static bool LCSF_EncodeAtt_Rec(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, uint_fast16_t attNb, const lcsf_raw_att_t *pAttArray) {
    // Parse the attribute array
    for (uint16_t attIdx = 0; attIdx < attNb; attIdx++) {
        // Guard against buffer overflow
        if (*pBuffIdx >= buffSize) {
            return false;
        }
        // Fill attribute header
        if (!LCSF_FillAttHeader(pBuffIdx, pBuffer, buffSize, &(pAttArray[attIdx]))) {
            return false;
        }
        // Test if attribute has sub-attributes
        if (pAttArray[attIdx].HasSubAtt) {
            // Encode current attribute sub-attribute array
            if (!LCSF_EncodeAtt_Rec(pBuffIdx, pBuffer, buffSize, pAttArray[attIdx].PayloadSize, pAttArray[attIdx].Payload.pSubAttArray)) {
                return false;
	         }
        } else {
            // Copy data into the buffer
            if (!LCSF_FillAttData(pBuffIdx, pBuffer, buffSize, &(pAttArray[attIdx]))) {
                return false;
            }
        }
    }
    return true;
}

/**
 * \fn static bool LCSF_BufferEncode(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_msg_t *pMsg)
 * \brief Encode lcsf_msg_header into a buffer
 *
 * \param pBuffIdx pointer to the buffer index
 * \param pBuffer pointer to the buffer
 * \param buffSize buffer size
 * \param pMsg pointer to the message header
 * \return bool: true if operation was a success
 */
static bool LCSF_EncodeBuffer(uint16_t *pBuffIdx, uint8_t *pBuffer, size_t buffSize, const lcsf_raw_msg_t *pMsg) {
    // Encode the message header
    if (!LCSF_FillMsgHeader(pBuffIdx, pBuffer, buffSize, pMsg)) {
        return false;
    }
    // Encode the attribute array
    if (!LCSF_EncodeAtt_Rec(pBuffIdx, pBuffer, buffSize, pMsg->AttNb, pMsg->pAttArray)) {
        return false;
    }
    return true;
}

// *** Public Functions ***

bool LCSF_TranscoderInit(void) {
    // Filo creation
    return FiloInit(&LcsfTranscoderInfo.DecoderFilo, LcsfTranscoderInfo.DecoderFiloData, LCSF_TRANSCODER_RX_FILO_SIZE, sizeof(lcsf_raw_att_t));
}

bool LCSF_TranscoderReceive(const uint8_t *pBuffer, size_t buffSize) {
    // Invalid parameters guards
    if (pBuffer == NULL) {
        return false;
    }
    lcsf_raw_msg_t *pMsg = &LcsfTranscoderInfo.DecoderMsg;
    // Decode buffer into lcsf object
    if (!LCSF_DecodeBuffer(pBuffer, buffSize, pMsg)) {
        // Encode error buffer
        LCSF_ValidatorSendTranscoderError(LcsfTranscoderInfo.LastErrCode);
        return false;
    }
    // Send lcsf object to receiver
    return LCSF_ValidatorReceive(pMsg);
}

int LCSF_TranscoderEncode(const lcsf_raw_msg_t *pMessage, uint8_t *pBuffer, size_t buffSize) {
    // Invalid parameters guards
    if (pMessage == NULL) {
        return -1;
    }
    if (pBuffer == NULL) {
        return -1;
    }
    uint16_t msgSize = 0;
    // Encode lcsf object into send buffer
    if (!LCSF_EncodeBuffer(&msgSize, pBuffer, buffSize, pMessage)) {
        return -1;
    }
    // Return encoded message size
    return msgSize;
}
