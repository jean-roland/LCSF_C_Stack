/**
 * @file       test_LCSF_Validator.cpp
 * @brief      Unit test of the LCSF_Validator module
 * @author     Jean-Roland Gosse
 *
 *             This file is part of LCSF C Stack.
 *
 *             LCSF C Stack is free software: you can redistribute it and/or
 *             modify it under the terms of the GNU General Public License as
 *             published by the Free Software Foundation, either version 3 of
 *             the License, or (at your option) any later version.
 *
 *             LCSF C Stack is distributed in the hope that it will be useful,
 *             but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *             General Public License for more details.
 *
 *             You should have received a copy of the GNU General Public License
 *             along with this project. If not, see
 *             <https://www.gnu.org/licenses/>
 */

// *** Private include ***
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "Example_Main.h"
#include "Filo.h"
#include "LCSF_Config.h"
#include "LCSF_Transcoder.h"
#include "LCSF_Validator.h"
#include "LCSF_Desc_Example.h"
#include "LCSF_Bridge_Example.h"
#include <string.h>
}

// *** Private macros ***
#define ERROR_CODE 0xff
#define TX_BUFF_SIZE 8

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
    0xFFFF,
    0x00,
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
    LCSF_EXAMPLE_CMD_ID_ERROR,
    LCSF_EXAMPLE_CMD_ERROR_ATT_NB,
    rxAttArr,
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
    DEBUG_PRINT("[tests]: Receive error function called %d %d\n", errorLoc, errorType);
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
 * testname: receive
 *
 * Test the receive feature
 */
TEST(LCSF_Validator, receive) {
    // Test message receive
    CHECK_FALSE(LCSF_ValidatorReceive(NULL));
    mock().expectOneCall("interpret_callback");
    CHECK(LCSF_ValidatorReceive(&rxMsg));

    // Test error receive
    mock().expectOneCall("recerr_callback");
    CHECK(LCSF_ValidatorReceive(&errMsg));
}