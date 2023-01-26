/**
 * @file       test_A_Test_Main.cpp
 * @brief      Unit test of the Test_Main (A) module
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
#include "LCSF_Bridge_Test.h"
#include "Test_Main.h"
#include <string.h>
}

// *** Private macros ***
#define ARRAY_SIZE 5
#define TX_BUFF_SIZE 8

// *** Private functions prototypes ***

// *** Private variables ***
static uint8_t txbuffer[TX_BUFF_SIZE];

// Test data
static uint8_t cc1_sa4_array[ARRAY_SIZE] = {6,5,4,3,2};
static uint8_t cc1_sa9_array[ARRAY_SIZE] = {2,3,4,5,6};
static char cc1_sa5_str[] = "Cpc";
static char cc1_sa10_str[] = "Qbvm";

static test_cmd_payload_t cc1_cmd_payload = {
    .cc1_payload = {
        .optAttFlagsBitfield = (TEST_CC1_ATT_SA6_FLAG | TEST_CC1_ATT_SA8_FLAG | TEST_CC1_ATT_SA9_FLAG | TEST_CC1_ATT_SA10_FLAG),
        .sa1 = 1,
        .sa2 = 2001,
        .sa3 = 100001,
        .sa4Size = ARRAY_SIZE,
        .p_sa4 = cc1_sa4_array,
        .p_sa5 = cc1_sa5_str,
        .sa6 = 4,
        .sa7 = 4001,
        .sa8 = 150000,
        .sa9Size = ARRAY_SIZE,
        .p_sa9 = cc1_sa9_array,
        .p_sa10 = cc1_sa10_str,
    },
};

static uint8_t cc2_sa4_array[ARRAY_SIZE] = {5,4,3,2,1};
static uint8_t cc2_sa9_array[ARRAY_SIZE] = {1,2,3,4,5};
static char cc2_sa5_str[] = "Bob";
static char cc2_sa10_str[] = "Paul";

static test_cmd_payload_t cc2_cmd_payload = {
    .cc2_payload = {
        .optAttFlagsBitfield = (TEST_CC2_ATT_SA6_FLAG | TEST_CC2_ATT_SA8_FLAG | TEST_CC2_ATT_SA9_FLAG | TEST_CC2_ATT_SA10_FLAG),
        .sa1 = 0,
        .sa2 = 2000,
        .sa3 = 100000,
        .sa4Size = ARRAY_SIZE,
        .p_sa4 = cc2_sa4_array,
        .p_sa5 = cc2_sa5_str,
        .sa6 = 3,
        .sa7 = 4000,
        .sa8 = 149999,
        .sa9Size = ARRAY_SIZE,
        .p_sa9 = cc2_sa9_array,
        .p_sa10 = cc2_sa10_str,
    },
};

static uint8_t ca4_sa4_array[ARRAY_SIZE] = {11,21,31,41,51};

static test_cmd_payload_t cc4_cmd_payload = {
    .cc4_payload = {
        .optAttFlagsBitfield = TEST_CC4_ATT_CA2_FLAG,
        .sa1 = 0,
        .ca1_payload = {
            .optAttFlagsBitfield = 0,
            .sa1 = 2,
            .sa2 = 2001,
            .sa3 = 100001,
        },
        .ca2_payload = {
            .optAttFlagsBitfield = 0,
            .sa1 = 3,
            .ca3_payload = {
                .optAttFlagsBitfield = TEST_CA3_ATT_SA1_FLAG,
                .sa1 = 4,
                .ca4_payload = {
                    .sa4Size = ARRAY_SIZE,
                    .p_sa4 = ca4_sa4_array,
                },
            },
        },
    },
};

static uint8_t ca8_sa4_array[ARRAY_SIZE] = {10,20,30,40,50};

static test_cmd_payload_t cc5_cmd_payload = {
    .cc5_payload = {
        .optAttFlagsBitfield = TEST_CC5_ATT_CA6_FLAG,
        .sa2 = 255,
        .ca5_payload = {
            .optAttFlagsBitfield = 0,
            .sa1 = 1,
            .sa2 = 2000,
            .sa3 = 100000,
        },
        .ca6_payload = {
            .optAttFlagsBitfield = 0,
            .sa1 = 2,
            .ca7_payload = {
                .optAttFlagsBitfield = TEST_CA7_ATT_SA1_FLAG,
                .sa1 = 3,
                .ca8_payload = {
                    .sa4Size = ARRAY_SIZE,
                    .p_sa4 = ca8_sa4_array,
                },
            },
        },
    },
};

static uint8_t pattern[ARRAY_SIZE] = {0x12,0x34,0x56,0x78,0x89};

// *** Private Functions ***

/**
 * @brief      Compare two test_cc1_att_payload_t
 *
 * @param[in]  pMsg1  Pointer to first payload
 * @param[in]  pMsg2  POinter to second payload
 *
 * @return     bool: true if successful
 */
static bool compare_payload_cc1(const test_cc1_att_payload_t *p1, const test_cc1_att_payload_t *p2) {
    if ((p1 == NULL) || (p2 == NULL)) {
        DEBUG_PRINT("[tests]: Received a null pointer !\n");
        return false;
    }
    if (p1->optAttFlagsBitfield != p2->optAttFlagsBitfield) {
        DEBUG_PRINT("[tests]: CC1: Different flag bitfield: %d, %d\n", p1->optAttFlagsBitfield, p2->optAttFlagsBitfield);
        return false;
    }
    if (p1->sa1 != p2->sa1) {
        DEBUG_PRINT("[tests]: CC1: Different SA1: %d, %d\n", p1->sa1, p2->sa1);
        return false;
    }
    if (p1->sa2 != p2->sa2) {
        DEBUG_PRINT("[tests]: CC1: Different SA2: %d, %d\n", p1->sa2, p2->sa2);
        return false;
    }
    if (p1->sa3 != p2->sa3) {
        DEBUG_PRINT("[tests]: CC1: Different SA3: %d, %d\n", p1->sa3, p2->sa3);
        return false;
    }
    if (p1->sa4Size != p2->sa4Size) {
        DEBUG_PRINT("[tests]: CC1: Different SA4 size: %d, %d\n", p1->sa4Size, p2->sa4Size);
        return false;
    }
    if ((p1->p_sa4 == NULL) || (p2->p_sa4 == NULL)) {
        DEBUG_PRINT("[tests]: CC1: Missing SA4\n");
        return false;
    }
    if (memcmp(p1->p_sa4, p2->p_sa4, p1->sa4Size) != 0) {
        DEBUG_PRINT("[tests]: CC1: Different SA4\n");
        return false;
    }
    if ((p1->p_sa5 == NULL) || (p2->p_sa5 == NULL)) {
        DEBUG_PRINT("[tests]: CC1: Missing SA5\n");
        return false;
    }
    if (strcmp(p1->p_sa5, p2->p_sa5) != 0) {
        DEBUG_PRINT("[tests]: CC1: Different SA5\n");
        return false;
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA6_FLAG) != 0) {
        if (p1->sa6 != p2->sa6) {
            DEBUG_PRINT("[tests]: CC1: Different SA6: %d, %d\n", p1->sa6, p2->sa6);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA7_FLAG) != 0) {
        if (p1->sa7 != p2->sa7) {
            DEBUG_PRINT("[tests]: CC1: Different SA7: %d, %d\n", p1->sa7, p2->sa7);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA8_FLAG) != 0) {
        if (p1->sa8 != p2->sa8) {
            DEBUG_PRINT("[tests]: CC1: Different SA8: %d, %d\n", p1->sa8, p2->sa8);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA9_FLAG) != 0) {
        if (p1->sa9Size != p2->sa9Size) {
            DEBUG_PRINT("[tests]: CC1: Different SA9 size: %d, %d\n", p1->sa9Size, p2->sa9Size);
            return false;
        }
        if ((p1->p_sa9 == NULL) || (p2->p_sa9 == NULL)) {
            DEBUG_PRINT("[tests]: CC1: Missing SA9\n");
            return false;
        }
        if (memcmp(p1->p_sa9, p2->p_sa9, p1->sa9Size) != 0) {
            DEBUG_PRINT("[tests]: CC1: Different SA9\n");
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA10_FLAG) != 0) {
        if ((p1->p_sa10 == NULL) || (p2->p_sa10 == NULL)) {
            DEBUG_PRINT("[tests]: CC1: Missing SA10\n");
            return false;
        }
        if (strcmp(p1->p_sa10, p2->p_sa10) != 0) {
            DEBUG_PRINT("[tests]: CC1: Different SA10\n");
            return false;
        }
    }
    return true;
}

/**
 * @brief      Compare two test_cc4_att_payload_t
 *
 * @param[in]  pMsg1  Pointer to first payload
 * @param[in]  pMsg2  POinter to second payload
 *
 * @return     bool: true if successful
 */
static bool compare_payload_cc4(const test_cc4_att_payload_t *p1, const test_cc4_att_payload_t *p2) {
    if ((p1 == NULL) || (p2 == NULL)) {
        DEBUG_PRINT("[tests]: Received a null pointer !\n");
        return false;
    }
    if (p1->optAttFlagsBitfield != p2->optAttFlagsBitfield) {
        DEBUG_PRINT("[tests]: CC4: Different flag bitfield: %d, %d\n", p1->optAttFlagsBitfield, p2->optAttFlagsBitfield);
        return false;
    }
    if (p1->sa1 != p2->sa1) {
        DEBUG_PRINT("[tests]: CC4: Different SA1: %d, %d\n", p1->sa1, p2->sa1);
        return false;
    }
    // CA1
    const test_cc4_att_ca1_att_payload_t *pca1_a = &p1->ca1_payload;
    const test_cc4_att_ca1_att_payload_t *pca1_b = &p2->ca1_payload;
    if (pca1_a->sa1 != pca1_b->sa1) {
        DEBUG_PRINT("[tests]: CC4_CA1: Different SA1: %d, %d\n", pca1_a->sa1, pca1_b->sa1);
        return false;
    }
    if (pca1_a->sa2 != pca1_b->sa2) {
        DEBUG_PRINT("[tests]: CC4_CA1: Different SA2: %d, %d\n", pca1_a->sa2, pca1_b->sa2);
        return false;
    }
    if ((pca1_a->optAttFlagsBitfield & TEST_CA1_ATT_SA3_FLAG) != 0) {
        if (pca1_a->sa3 != pca1_b->sa3) {
            DEBUG_PRINT("[tests]: CC4_CA1: Different SA3: %d, %d\n", pca1_a->sa3, pca1_b->sa3);
            return false;
        }
    }
    // CA2
    if ((p1->optAttFlagsBitfield & TEST_CC4_ATT_CA2_FLAG) != 0) {
        const test_cc4_att_ca2_att_payload_t *pca2_a = &p1->ca2_payload;
        const test_cc4_att_ca2_att_payload_t *pca2_b = &p2->ca2_payload;

        if ((pca2_a->optAttFlagsBitfield & TEST_CA2_ATT_SA1_FLAG) != 0) {
            if (pca2_a->sa1 != pca2_b->sa1) {
                DEBUG_PRINT("[tests]: CC4_CA2: Different SA1: %d, %d\n", pca2_a->sa1, pca2_b->sa1);
                return false;
            }
        }
        // CA3
        const test_ca2_att_ca3_att_payload_t *pca3_a = &pca2_a->ca3_payload;
        const test_ca2_att_ca3_att_payload_t *pca3_b = &pca2_b->ca3_payload;

        if ((pca3_a->optAttFlagsBitfield & TEST_CA3_ATT_SA1_FLAG) != 0) {
            if (pca3_a->sa1 != pca3_b->sa1) {
                DEBUG_PRINT("[tests]: CC4_CA3: Different SA1: %d, %d\n", pca3_a->sa1, pca3_b->sa1);
                return false;
            }
        }
        // CA4
        const test_ca3_att_ca4_att_payload_t *pca4_a = &pca3_a->ca4_payload;
        const test_ca3_att_ca4_att_payload_t *pca4_b = &pca3_b->ca4_payload;

        if (pca4_a->sa4Size != pca4_b->sa4Size) {
            DEBUG_PRINT("[tests]: CC4_CA4: Different SA4 size: %d, %d\n", pca4_a->sa4Size, pca4_b->sa4Size);
            return false;
        }
        if ((pca4_a->p_sa4 == NULL) || (pca4_b->p_sa4 == NULL)) {
            DEBUG_PRINT("[tests]: CC4_CA4: Missing SA4\n");
            return false;
        }
        if (memcmp(pca4_a->p_sa4, pca4_b->p_sa4, pca4_a->sa4Size) != 0) {
            DEBUG_PRINT("[tests]: CC4_CA4: Different SA4\n");
            return false;
        }
    }
    return true;
}

/**
 * @brief      Expect a LCSF_Bridge_TestEncode call helper function
 *
 * @param[in]  cmdName     Expected command name
 * @param[in]  hasPattern  Indicates if pattern
 */
static void ExpectEncode(uint_fast16_t cmdName, bool hasPattern) {
    mock().setData("hasPattern", hasPattern);
    mock().expectOneCall("LCSF_Bridge_TestEncode").withParameter("cmdName", cmdName);
}

// *** Public Functions ***

/**
 * @brief      mock LCSF_Bridge_TestEncode function
 *
 * @param[in]  cmdName      The command name
 * @param      pCmdPayload  The command payload
 * @param      pBuffer      The buffer
 * @param[in]  buffSize     The buffer size
 *
 * @return     int: -1 if operation fail, encoded message size if success
 */
int LCSF_Bridge_TestEncode(uint_fast16_t cmdName, test_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize) {
    DEBUG_PRINT("[tests]: Received send cmd: %ld\n", cmdName);
    bool hasPattern = mock().getData("hasPattern").getBoolValue();
    mock().actualCall("LCSF_Bridge_TestEncode").withParameter("cmdName", cmdName);
    // Process cmdName for return
    switch (cmdName) {
        case TEST_CMD_SC1:
            return TX_BUFF_SIZE;
        case TEST_CMD_CC1: {
            if (hasPattern) {
                test_cc1_att_payload_t *p = &pCmdPayload->cc1_payload;
                MEMCMP_EQUAL(p->p_sa4, pattern, ARRAY_SIZE);
                MEMCMP_EQUAL(p->p_sa9, pattern, ARRAY_SIZE);
                return TX_BUFF_SIZE;
            }
            if (!compare_payload_cc1(&pCmdPayload->cc1_payload, &cc1_cmd_payload.cc1_payload)) {
                return -1;
            }
            return TX_BUFF_SIZE;
        }
        case TEST_CMD_CC4:
            if (!compare_payload_cc4(&pCmdPayload->cc4_payload, &cc4_cmd_payload.cc4_payload)) {
                return -1;
            }
            return TX_BUFF_SIZE;
        default:
            return -1;
    }
}

// *** Tests ***

/**
 * testgroup: A_Test_Main
 *
 * This group tests all the Test_Main (A) features
 */
TEST_GROUP(A_Test_Main) {
    void setup() {
        mock().strictOrder();
        // Init module
        CHECK_FALSE(Test_MainInit(NULL, 0))
        CHECK(Test_MainInit(txbuffer, TX_BUFF_SIZE));
    }
    void teardown() {
        mock().checkExpectations();
        mock().clear();
    }
};

/**
 * testgroup: A_Test_Main
 * testname: execute
 *
 * Test the send/encode features
 */
TEST(A_Test_Main, execute)  {
    // Test error cases
    CHECK_FALSE(Test_MainExecute(TEST_CMD_COUNT, NULL));
    CHECK_FALSE(Test_MainExecute(TEST_CMD_CC2, NULL));
    // Test valid cases
    ExpectEncode(TEST_CMD_SC1, false);
    CHECK(Test_MainExecute(TEST_CMD_SC2, NULL));
    ExpectEncode(TEST_CMD_CC1, false);
    CHECK(Test_MainExecute(TEST_CMD_CC2, &cc2_cmd_payload));
    ExpectEncode(TEST_CMD_CC4, false);
    CHECK(Test_MainExecute(TEST_CMD_CC5, &cc5_cmd_payload));
    // Test pattern
    Test_MainSetPattern(pattern);
    ExpectEncode(TEST_CMD_CC1, true);
    CHECK(Test_MainExecute(TEST_CMD_CC2, &cc2_cmd_payload));
}