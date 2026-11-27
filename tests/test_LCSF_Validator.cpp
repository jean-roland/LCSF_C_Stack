/**
 * @file test_LCSF_Validator.cpp
 * @brief Unit test of the LCSF_Validator module
 * @author Jean-Roland Gosse
 *
 * This file is part of LCSF C Stack.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>
 */

// *** Private include ***
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <string.h>
#include "Lifo.h"
#include "LCSF_Config.h"
#include "LCSF_Transcoder.h"
#include "LCSF_Validator.h"
#include "LCSF_Bridge_Example.h"
#include "Example_Main.h"
}

// *** Private macros ***
#define ERROR_CODE 0xff
#define TX_BUFF_SIZE 8
// Error protocol id, matching the transcoder's per-representation id
#ifdef LCSF_SMALL
#define ERR_PROT_ID 0x00FF
#else
#define ERR_PROT_ID 0xFFFF
#endif

// *** Private functions prototypes ***
static bool compare_validatt(const lcsf_valid_att_t *pAtt1, const lcsf_valid_att_t *pAtt2);
static bool compare_validmsg(const lcsf_valid_cmd_t *pMsg1, const lcsf_valid_cmd_t *pMsg2);
static bool compare_rawatt(const lcsf_raw_att_t *pAtt1, const lcsf_raw_att_t *pAtt2);
static bool compare_rawmsg(const lcsf_raw_msg_t *pMsg1, const lcsf_raw_msg_t *pMsg2);
static void *malloc_Callback(size_t size, int num_calls);
static bool process_Callback(const lcsf_raw_msg_t *pMsg, int num_calls);
static bool interpret_callback(lcsf_valid_cmd_t *pValidCmd);

// *** Private variables ***
static uint8_t txBuffer[TX_BUFF_SIZE];
static uint8_t data_errloc = 0x00;
static uint8_t data_errcode = ERROR_CODE;
static uint8_t att1Data[] = {0x04};

// Example protocol descriptor
static const lcsf_validator_protocol_desc_t example_prot_desc = {
    LCSF_EXAMPLE_PROTOCOL_ID,
    LCSF_EXAMPLE_PROTOCOL_VERSION,
    &LCSF_Example_ProtDesc,
    interpret_callback,
};

// Test data

// Model error msg
static lcsf_raw_att_t errAttArr[] = {
    {
        0x00,
        false,
        sizeof(uint8_t),
        {
            .pData = &data_errloc,
        }
    },
    {
        0x01,
        false,
        sizeof(uint8_t),
        {
            .pData = &data_errcode,
        }
    },
};

static const lcsf_raw_msg_t errMsg = {
    ERR_PROT_ID, // ProtId
    0x00, // ProtVer (LCSF error protocol version)
    0x00,
    2,
    errAttArr,
};

// Model error msg carrying an unexpected command id (must be ignored, not processed)
static const lcsf_raw_msg_t badCmdErrMsg = {
    ERR_PROT_ID, // ProtId
    0x00, // ProtVer (LCSF error protocol version)
    0x01, // CmdId (not LCSF_EP_CMD_ERROR_ID)
    2,
    errAttArr,
};

// Model raw msg
static lcsf_raw_att_t rxAttArr[] = {
    {
        LCSF_EXAMPLE_ATT_ERROR_CODE,
        false,
        sizeof(att1Data),
        {
            .pData = att1Data,
        }
    },
};

static const lcsf_raw_msg_t rxMsg = {
    LCSF_EXAMPLE_PROTOCOL_ID,
    LCSF_EXAMPLE_PROTOCOL_VERSION,
    LCSF_EXAMPLE_CMD_ID_ERROR,
    LCSF_EXAMPLE_CMD_ERROR_ATT_NB,
    rxAttArr,
};

// Model raw msg with a mismatched protocol version
static const lcsf_raw_msg_t badVerMsg = {
    LCSF_EXAMPLE_PROTOCOL_ID,
    LCSF_EXAMPLE_PROTOCOL_VERSION + 1,
    LCSF_EXAMPLE_CMD_ID_ERROR,
    LCSF_EXAMPLE_CMD_ERROR_ATT_NB,
    rxAttArr,
};

// Model error msg expected when a protocol version mismatch is detected
static uint8_t data_badver_errloc = 0x01; // LCSF_EP_ERROR_LOC_VALIDATION_ERROR
static uint8_t data_badver_errcode = 0x06; // LCSF_EP_ERROR_CODE_BAD_PROT_VER
static lcsf_raw_att_t badVerErrAttArr[] = {
    {
        0x00,
        false,
        sizeof(uint8_t),
        {
            .pData = &data_badver_errloc,
        }
    },
    {
        0x01,
        false,
        sizeof(uint8_t),
        {
            .pData = &data_badver_errcode,
        }
    },
};
static const lcsf_raw_msg_t badVerErrMsg = {
    ERR_PROT_ID, // ProtId
    0x00, // ProtVer (LCSF error protocol version)
    0x00,
    2,
    badVerErrAttArr,
};

// Model raw msg with a data attribute sent with sub-attributes, its payload size is then their count
static lcsf_raw_att_t subAttOnDataSubAttArr[] = {
    {
        0x00,
        false,
        sizeof(att1Data),
        {
            .pData = att1Data,
        }
    },
};
static lcsf_raw_att_t subAttOnDataAttArr[] = {
    {
        LCSF_EXAMPLE_ATT_ERROR_CODE,
        true,
        sizeof(subAttOnDataSubAttArr) / sizeof(lcsf_raw_att_t),
        {
            .pSubAttArray = subAttOnDataSubAttArr,
        }
    },
};
static const lcsf_raw_msg_t subAttOnDataMsg = {
    LCSF_EXAMPLE_PROTOCOL_ID,
    LCSF_EXAMPLE_PROTOCOL_VERSION,
    LCSF_EXAMPLE_CMD_ID_ERROR,
    LCSF_EXAMPLE_CMD_ERROR_ATT_NB,
    subAttOnDataAttArr,
};

// Model error msg expected when an attribute has the wrong data type
static uint8_t data_wrongtype_errcode = 0x05; // LCSF_EP_ERROR_CODE_WRONG_ATT_DATA_TYPE
static lcsf_raw_att_t wrongTypeErrAttArr[] = {
    {
        0x00,
        false,
        sizeof(uint8_t),
        {
            .pData = &data_badver_errloc,
        }
    },
    {
        0x01,
        false,
        sizeof(uint8_t),
        {
            .pData = &data_wrongtype_errcode,
        }
    },
};
static const lcsf_raw_msg_t wrongTypeErrMsg = {
    ERR_PROT_ID, // ProtId
    0x00, // ProtVer (LCSF error protocol version)
    0x00,
    2,
    wrongTypeErrAttArr,
};


// Model valid msg
static lcsf_valid_att_t txAttArr[] = {
    sizeof(att1Data),
    {
        .pData = att1Data,
    }
};

static const lcsf_valid_cmd_t txMsg = {
    LCSF_EXAMPLE_CMD_ID_ERROR,
    txAttArr,
};

// Byte array and string protocol, payloads one byte over the size field
#define BIG_PROT_ID 0x01
#define BIG_CMD_ID_ARRAY 0x00
#define BIG_CMD_ID_STRING 0x01

static const lcsf_attribute_desc_t bigArrayAttDescArray[] = {
    {false, LCSF_BYTE_ARRAY, 0x00, 0, NULL},
};

static const lcsf_attribute_desc_t bigStringAttDescArray[] = {
    {false, LCSF_STRING, 0x00, 0, NULL},
};

static const lcsf_command_desc_t bigCmdDescArray[] = {
    {BIG_CMD_ID_ARRAY, 1, bigArrayAttDescArray},
    {BIG_CMD_ID_STRING, 1, bigStringAttDescArray},
};

static const lcsf_protocol_desc_t bigProtDesc = {
    sizeof(bigCmdDescArray)/sizeof(lcsf_command_desc_t),
    bigCmdDescArray,
};

static const lcsf_validator_protocol_desc_t big_prot_desc = {
    BIG_PROT_ID,
    0x00,
    &bigProtDesc,
    interpret_callback,
};

static uint8_t bigArrayData[LCSF_ATT_PAYLOAD_MAX_SIZE + 1];
static char bigStringData[LCSF_ATT_PAYLOAD_MAX_SIZE + 1]; // Filled up to its last nul in the test

static lcsf_valid_att_t bigArrayAttArr[] = {
    sizeof(bigArrayData),
    {
        .pData = bigArrayData,
    }
};

static lcsf_valid_att_t bigStringAttArr[] = {
    sizeof(bigStringData),
    {
        .pData = bigStringData,
    }
};

static const lcsf_valid_cmd_t bigArrayMsg = {
    BIG_CMD_ID_ARRAY,
    bigArrayAttArr,
};

static const lcsf_valid_cmd_t bigStringMsg = {
    BIG_CMD_ID_STRING,
    bigStringAttArr,
};

// *** Private Functions ***

/**
 * @brief      Compares two lcsf_valid_att_t
 *
 * @param[in]  pAtt1  Pointer to first valid attribute
 * @param[in]  pAtt2  Pointer to second valid attribute
 *
 * @return     bool: true if they are equal
 */
static bool compare_validatt(const lcsf_valid_att_t *pAtt1, const lcsf_valid_att_t *pAtt2) {
    if (pAtt1->PayloadSize != pAtt2->PayloadSize) {
        return false;
    }
    if (memcmp(pAtt1->Payload.pData, pAtt2->Payload.pData, pAtt1->PayloadSize) != 0) {
        return false;
    }
    return true;
}

/**
 * @brief      Compares two lcsf_valid_cmd_t.
 *
 * @param[in]  pMsg1  Pointer to first valid command
 * @param[in]  pMsg2  Pointer to second valid command
 *
 * @return     bool: true if they are equal
 */
static bool compare_validmsg(const lcsf_valid_cmd_t *pMsg1, const lcsf_valid_cmd_t *pMsg2) {
    if (pMsg1->CmdId != pMsg2->CmdId) {
        return false;
    }
    for (uint16_t idx = 0; idx < LCSF_EXAMPLE_CMD_ERROR_ATT_NB; idx++) {
        if (!compare_validatt(&pMsg1->pAttArray[idx], &pMsg2->pAttArray[idx])) {
            return false;
        }
    }
    return true;
}

/**
 * @brief      Compares two lcsf_raw_att_t
 *
 * @param[in]  pAtt1  Pointer to first raw attribute
 * @param[in]  pAtt2  Pointer to second raw attribute
 *
 * @return     bool: true if they are equal
 */
static bool compare_rawatt(const lcsf_raw_att_t *pAtt1, const lcsf_raw_att_t *pAtt2) {
    if (pAtt1->AttId != pAtt2->AttId) {
        return false;
    }
    if (pAtt1->HasSubAtt != pAtt2->HasSubAtt) {
        return false;
    }
    if (pAtt1->PayloadSize != pAtt2->PayloadSize) {
        return false;
    }
    if (pAtt1->HasSubAtt) {
        for (uint16_t idx = 0; idx < pAtt1->PayloadSize; idx++) {
            if (!compare_rawatt(&pAtt1->Payload.pSubAttArray[idx], &pAtt2->Payload.pSubAttArray[idx])) {
                return false;
            }
        }
    } else {
        if (memcmp(pAtt1->Payload.pData, pAtt2->Payload.pData, pAtt1->PayloadSize) != 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief      Compares two lcsf_raw_msg_t
 *
 * @param[in]  pMsg1  Pointer to first raw message
 * @param[in]  pMsg2  Pointer to second raw message
 *
 * @return     bool: true if they are equal
 */
static bool compare_rawmsg(const lcsf_raw_msg_t *pMsg1, const lcsf_raw_msg_t *pMsg2) {
    if (pMsg1->ProtVer != pMsg2->ProtVer) {
        return false;
    }
    if (pMsg1->ProtId != pMsg2->ProtId) {
        return false;
    }
    if (pMsg1->CmdId != pMsg2->CmdId) {
        return false;
    }
    if (pMsg1->AttNb != pMsg2->AttNb) {
        return false;
    }
    for (uint16_t idx = 0; idx < pMsg1->AttNb; idx++) {
        if (!compare_rawatt(&pMsg1->pAttArray[idx], &pMsg2->pAttArray[idx])) {
            return false;
        }
    }
    return true;
}

/**
 * @brief      Function called by LCSF_Validator to interpret commands
 *
 * @param      pValidCmd  Pointer to valid command
 *
 * @return     bool: true if operation successful
 */
static bool interpret_callback(lcsf_valid_cmd_t *pValidCmd) {
    mock().actualCall("interpret_callback");
    return compare_validmsg(pValidCmd, &txMsg);
}

/**
 * @brief      Function called by LCSF_Validator to send error messages
 *
 * @param[in]  pBuffer   The buffer
 * @param[in]  buffSize  The buffer size
 *
 * @return     bool: true if operation successful
 */
static bool senderr_callback(const uint8_t *pBuffer, size_t buffSize) {
    mock().actualCall("senderr_callback");
    return true;
}

/**
 * @brief      Function called by LCSF_Validator when error messages are received
 *
 * @param[in]  errorLoc   The error location
 * @param[in]  errorType  The error type
 */
static void recerr_callback(uint_fast8_t errorLoc, uint_fast8_t errorType) {
    mock().actualCall("recerr_callback");
    LCSF_DBG_PRINT("[tests]: Receive error function called %d %d\n", errorLoc, errorType);
}

/**
 * @brief      Expect a LCSF_TranscoderEncode call helper function
 *
 * @param[in]  pRefMsg  Pointer to reference message
 */
static void ExpectEncode(const lcsf_raw_msg_t *pRefMsg) {
    mock().setData("pRefMsg", (void *)pRefMsg);
    mock().expectOneCall("LCSF_TranscoderEncode");
}

// *** Mock Functions ***

/**
 * @brief      mock LCSF_TranscoderEncode function
 *
 * @param[in]  pMessage  The message
 * @param      pBuffer   The buffer
 * @param[in]  buffSize  The buffer size
 *
 * @return     int: -1 if error, encoded message size if success
 */
int LCSF_TranscoderEncode(const lcsf_raw_msg_t *pMessage, uint8_t* pBuffer, size_t buffSize) {
    mock().actualCall("LCSF_TranscoderEncode");
    // Retrieve reference message
    const lcsf_raw_msg_t *pRefMsg = (const lcsf_raw_msg_t *)mock().getData("pRefMsg").getPointerValue();
    // Compare produced message with reference
    if (!compare_rawmsg(pMessage, pRefMsg)) {
        return -1;
    }
    return 0; // dummy value
}

// *** Tests ***

/**
 * testgroup: LCSF_Validator
 *
 * This group tests all the LCSF_Validator features
 */
TEST_GROUP(LCSF_Validator) {
    void setup() {
        mock().strictOrder();
        // Init module
        CHECK(LCSF_ValidatorInit(&senderr_callback, &recerr_callback));
        CHECK_FALSE(LCSF_ValidatorAddProtocol(0, NULL));
        CHECK(LCSF_ValidatorAddProtocol(0, &example_prot_desc));
    }
    void teardown() {
        mock().checkExpectations();
        mock().clear();
    }
};

/**
 * testgroup: LCSF_Validator
 * testname: encode
 *
 * Test the encode feature
 */
TEST(LCSF_Validator, encode)  {
    ExpectEncode(&errMsg);
    mock().expectOneCall("senderr_callback");
    CHECK(LCSF_ValidatorSendTranscoderError(ERROR_CODE));
    CHECK_EQUAL(LCSF_ValidatorEncode(LCSF_EXAMPLE_PROTOCOL_ID, NULL, txBuffer, TX_BUFF_SIZE), -1);
    ExpectEncode(&rxMsg);
    CHECK_EQUAL(LCSF_ValidatorEncode(LCSF_EXAMPLE_PROTOCOL_ID, &txMsg, txBuffer, TX_BUFF_SIZE), 0);
}

/**
 * testgroup: LCSF_Validator
 * testname: encode_oversize
 *
 * Test the encode feature refuses a payload its size field can't hold, before calling the transcoder
 */
TEST(LCSF_Validator, encode_oversize) {
    CHECK(LCSF_ValidatorAddProtocol(1, &big_prot_desc));
    memset(bigStringData, 'a', sizeof(bigStringData) - 1);
    CHECK_EQUAL(LCSF_ValidatorEncode(BIG_PROT_ID, &bigArrayMsg, txBuffer, TX_BUFF_SIZE), -1);
    CHECK_EQUAL(LCSF_ValidatorEncode(BIG_PROT_ID, &bigStringMsg, txBuffer, TX_BUFF_SIZE), -1);
}

/**
 * testgroup: LCSF_Validator
 * testname: receive
 *
 * Test the receive feature
 */
TEST(LCSF_Validator, receive) {
    // Test message receive
    CHECK(LCSF_ValidatorReceive(NULL) != LCSF_RECEIVE_OK);
    mock().expectOneCall("interpret_callback");
    CHECK(LCSF_ValidatorReceive(&rxMsg) == LCSF_RECEIVE_OK);

    // Test protocol version mismatch returns the dedicated status (and sends a bad version error back)
    ExpectEncode(&badVerErrMsg);
    mock().expectOneCall("senderr_callback");
    CHECK(LCSF_ValidatorReceive(&badVerMsg) == LCSF_RECEIVE_BAD_PROT_VER);

    // Test a data attribute sent with sub-attributes is rejected (and sends a wrong data type error back)
    ExpectEncode(&wrongTypeErrMsg);
    mock().expectOneCall("senderr_callback");
    CHECK(LCSF_ValidatorReceive(&subAttOnDataMsg) != LCSF_RECEIVE_OK);

    // Test error receive
    mock().expectOneCall("recerr_callback");
    CHECK(LCSF_ValidatorReceive(&errMsg) == LCSF_RECEIVE_OK);

    // Test error frame with an unexpected command id is ignored (no callback, no error sent)
    CHECK(LCSF_ValidatorReceive(&badCmdErrMsg) != LCSF_RECEIVE_OK);
}
