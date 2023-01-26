/**
 * @file       test_LCSF_Transcoder.cpp
 * @brief      Unit test of the LCSF_Transcoder module
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
#include "LCSF_Config.h"
#include "LCSF_Transcoder.h"
#include "LCSF_Validator.h"
#include "Filo.h"
#include <string.h>
}

// *** Private macros ***
#define TX_BUFF_SIZE 64

// *** Private functions prototypes ***
static bool compare_rawatt(const lcsf_raw_att_t *pAtt1, const lcsf_raw_att_t *pAtt2);
static bool compare_rawmsg(const lcsf_raw_msg_t *pMsg1, const lcsf_raw_msg_t *pMsg2);

// *** Private variables ***
static uint8_t txBuffer[TX_BUFF_SIZE];

// Test data
#ifdef LCSF_SMALL
    static const uint8_t ovrflwMsg[] = {0xaa, 0x01, 0x20};
    static const uint8_t badformatMsg[] = {0xaa, 0x01, 0x0a};
    static const uint8_t rxMsg[] = {
        0xab, 0x12, 0x03, 0x55, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04,
        0xff, 0x02, 0x30, 0x01, 0x0a, 0xb1, 0x01, 0x32, 0x02, 0xab,
        0xcd, 0x40, 0x10, 0x4f, 0x72, 0x67, 0x61, 0x6e, 0x6f, 0x6c,
        0x65, 0x70, 0x74, 0x69, 0x63, 0x00, 0x00, 0x00, 0x00
    };
#else
    static const uint8_t ovrflwMsg[] = {0xaa, 0x00, 0x01, 0x00, 0x20, 0x00};
    static const uint8_t badformatMsg[] = {0xaa, 0x00, 0x01, 0x00, 0x0a, 0x00};
    static const uint8_t rxMsg[] = {
        0xab, 0x00, 0x00, 0x12, 0x03, 0x00, 0x55, 0x00, 0x05, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04,
        0xff, 0xff, 0x02, 0x00, 0x30, 0x00, 0x01, 0x00, 0x0a, 0x31, 0x80, 0x01, 0x00, 0x32, 0x00,
        0x02, 0x00, 0xab, 0xcd, 0x40, 0x00, 0x10, 0x00, 0x4f, 0x72, 0x67, 0x61, 0x6e, 0x6f, 0x6c,
        0x65, 0x70, 0x74, 0x69, 0x63, 0x00, 0x00, 0x00, 0x00
    };
#endif

// Model raw msg attributes
static uint8_t txAtt1Data[] = {0x00, 0x01, 0x02, 0x03, 0x04};
static uint8_t txAtt2Data[] = {0xa};
static uint8_t txAtt3Data[] = {0xab, 0xcd};
static uint8_t txAtt4Data[] = { // Organoleptic
    0x4f, 0x72, 0x67, 0x61, 0x6e, 0x6f, 0x6c, 0x65,
    0x70, 0x74, 0x69, 0x63, 0x00, 0x00, 0x00, 0x00
};

static lcsf_raw_att_t txSubSubAtt[] = {
    {
        0x32,
        false,
        sizeof(txAtt3Data),
        {
            .pData = txAtt3Data,
        }
    }
};

static lcsf_raw_att_t txSubAttArr[] = {
    {
        0x30,
        false,
        sizeof(txAtt2Data),
        {
            .pData = txAtt2Data,
        }
    },
    {
        0x31,
        true,
        sizeof(txSubSubAtt)/sizeof(lcsf_raw_att_t),
        {
            .pSubAttArray = txSubSubAtt,
        }
    },
};

static lcsf_raw_att_t txAttArr[] = {
    {
        0x55,
        false,
        sizeof(txAtt1Data),
        {
            .pData = txAtt1Data,
        }
    },
    {
#ifdef LCSF_SMALL
        0x7f,
#else
        0x7fff,
#endif
        true,
        sizeof(txSubAttArr)/sizeof(lcsf_raw_att_t),
        {
            .pSubAttArray = txSubAttArr,
        }
    },
    {
        0x40,
        false,
        sizeof(txAtt4Data),
        {
            .pData = txAtt4Data,
        }
    },
};

static const lcsf_raw_msg_t txMsg = {
    0xab,
#ifdef LCSF_SMALL
        0x12,
#else
        0x1200,
#endif
    sizeof(txAttArr)/sizeof(lcsf_raw_att_t),
    txAttArr
};

// *** Private Functions ***

/**
 * @brief      Compare two lcsf_raw_att_t
 *
 * @param[in]  pAtt1  Pointer to first attribute
 * @param[in]  pAtt2  Pointer to second attribute
 *
 * @return     { description_of_the_return_value }
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
 * @brief      Compare two lcsf_raw_msg_t
 *
 * @param[in]  pMsg1  Pointer to first message
 * @param[in]  pMsg2  POinter to second message
 *
 * @return     bool: true if successful
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
 * @brief      Expect a LCSF_ValidatorSendTranscoderError call helper function
 *
 * @param[in]  errorType  The error type
 * @param[in]  isOk       Mock return value
 */
static void ExpectSendError(uint_fast8_t errorType, bool isOk) {
    mock().expectOneCall("LCSF_ValidatorSendTranscoderError").withParameter("errorType", errorType).andReturnValue(isOk);
}

/**
 * @brief      Expect a LCSF_ValidatorReceive call helper function
 */
static void ExpectReceive(void) {
    mock().expectOneCall("LCSF_ValidatorReceive");
}

// *** Mock Functions ***

/**
 * @brief      mock LCSF_ValidatorSendTranscoderError function
 *
 * @param[in]  errorType  The error type
 *
 * @return     bool: true if successful
 */
bool LCSF_ValidatorSendTranscoderError(uint_fast8_t errorType) {
    mock().actualCall("LCSF_ValidatorSendTranscoderError").withParameter("errorType", errorType);
    return mock().returnBoolValueOrDefault(false);
}

/**
 * @brief      mock LCSF_ValidatorReceive function
 *
 * @param[in]  pMessage  Pointer to the message
 *
 * @return     bool: true if successful
 */
bool LCSF_ValidatorReceive(const lcsf_raw_msg_t *pMessage) {
    mock().actualCall("LCSF_ValidatorReceive");
    return compare_rawmsg(pMessage, &txMsg);
}

// *** Tests ***

/**
 * testgroup: LCSF_Transcoder
 *
 * This group tests all the LCSF_Transcoder features
 */
TEST_GROUP(LCSF_Transcoder) {
    void setup() {
        mock().strictOrder();
        // Init module
        CHECK_TEXT(LCSF_TranscoderInit(), "[tests]: Failed to init transcoder module\n");
    }
    void teardown() {
        mock().checkExpectations();
        mock().clear();
    }
};

/**
 * testgroup: LCSF_Transcoder
 * testname: encode
 *
 * Test the encode feature
 */
TEST(LCSF_Transcoder, encode) {
    // Test error cases
    CHECK_EQUAL(LCSF_TranscoderEncode(NULL, txBuffer, TX_BUFF_SIZE), -1);
    CHECK_EQUAL(LCSF_TranscoderEncode(&txMsg, NULL, TX_BUFF_SIZE), -1);

    // Test valid message
#ifdef LCSF_SMALL
    DEBUG_PRINT("[tests]: Smaller LCSF representation is in use.\n");
    CHECK_EQUAL(LCSF_TranscoderEncode(&txMsg, txBuffer, TX_BUFF_SIZE), 36);
#else
    DEBUG_PRINT("[tests]: Regular LCSF representation is in use.\n");
    CHECK_EQUAL(LCSF_TranscoderEncode(&txMsg, txBuffer, TX_BUFF_SIZE), 54);
#endif
    MEMCMP_EQUAL(txBuffer, rxMsg, sizeof(rxMsg));
}

/**
 * testgroup: LCSF_Transcoder
 * testname: receive
 *
 * Test the receive feature
 */
TEST(LCSF_Transcoder, receive) {
    // Test error cases
    CHECK_FALSE(LCSF_TranscoderReceive(NULL, 0));
    ExpectSendError(0x01, true);
    CHECK_FALSE(LCSF_TranscoderReceive(ovrflwMsg, sizeof(ovrflwMsg)));
    ExpectSendError(0x00, true);
    CHECK_FALSE(LCSF_TranscoderReceive(badformatMsg, sizeof(badformatMsg)));
    // Test valid packet
    ExpectReceive();
    CHECK(LCSF_TranscoderReceive(rxMsg, sizeof(rxMsg)));
}