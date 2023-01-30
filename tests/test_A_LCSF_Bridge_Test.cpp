/**
 * @file       test_A_LCSF_Bridge_Test.cpp
 * @brief      Unit test of the LCSF_Bridge_Test (A) module
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
#include <string.h>
#include "Filo.h"
#include "LCSF_Bridge_Test.h"
#include "LCSF_Config.h"
#include "LCSF_Validator.h"
#include "Test_Main.h"
}

// *** Private macros ***
#define ARRAY_SIZE 5
#define TX_BUFF_SIZE 8

// *** Private functions prototypes ***
static bool compare_valid_cc1(const lcsf_valid_cmd_t *p1, const lcsf_valid_cmd_t *p2);
static bool compare_valid_cc4(const lcsf_valid_cmd_t *p1, const lcsf_valid_cmd_t *p2);
static bool compare_payload_cc2(const test_cc2_att_payload_t *p1, const test_cc2_att_payload_t *p2);
static bool compare_payload_cc5(const test_cc5_att_payload_t *p1, const test_cc5_att_payload_t *p2);
static void ExpectEncode(uint_fast16_t refCmdId);
static void ExpectExecute(uint_fast16_t cmdName);

// *** Private variables ***
static uint8_t txbuffer[TX_BUFF_SIZE];

// Test data

// Model payload
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

// Model valid msg
static lcsf_valid_cmd_t sc2_msg = {
    LCSF_TEST_CMD_ID_SC2,
    NULL,
};

static uint8_t cc1_sa1 = 1;
static uint16_t cc1_sa2 = 2001;
static uint32_t cc1_sa3 = 100001;
static uint8_t cc1_sa4[ARRAY_SIZE] = {6,5,4,3,2};
static char cc1_sa5[] = "Cpc";
static uint8_t cc1_sa6 = 4;
static uint16_t cc1_sa7 = 4001;
static uint32_t cc1_sa8 = 150000;
static uint8_t cc1_sa9[ARRAY_SIZE] = {2,3,4,5,6};
static char cc1_sa10[] = "Qbvm";

static lcsf_valid_att_t cc1_att_array[] = {
    {sizeof(cc1_sa1), {.pData = &cc1_sa1}},
    {sizeof(cc1_sa2), {.pData = &cc1_sa2}},
    {sizeof(cc1_sa3), {.pData = &cc1_sa3}},
    {sizeof(cc1_sa4), {.pData = &cc1_sa4}},
    {sizeof(cc1_sa5), {.pData = &cc1_sa5}},
    {sizeof(cc1_sa6), {.pData = &cc1_sa6}},
    {sizeof(cc1_sa7), {.pData = NULL}},
    {sizeof(cc1_sa8), {.pData = &cc1_sa8}},
    {sizeof(cc1_sa9), {.pData = &cc1_sa9}},
    {sizeof(cc1_sa10), {.pData = &cc1_sa10}},
};

static lcsf_valid_cmd_t cc1_msg = {
    LCSF_TEST_CMD_ID_CC1,
    cc1_att_array,
};

static uint8_t cc2_sa1 = 0;
static uint16_t cc2_sa2 = 2000;
static uint32_t cc2_sa3 = 100000;
static uint8_t cc2_sa4[ARRAY_SIZE] = {5,4,3,2,1};
static char cc2_sa5[] = "Bob";
static uint8_t cc2_sa6 = 3;
static uint16_t cc2_sa7 = 4000;
static uint32_t cc2_sa8 = 149999;
static uint8_t cc2_sa9[ARRAY_SIZE] = {1,2,3,4,5};
static char cc2_sa10[] = "Paul";

static lcsf_valid_att_t cc2_att_array[] = {
    {sizeof(cc2_sa1), {.pData = &cc2_sa1}},
    {sizeof(cc2_sa2), {.pData = &cc2_sa2}},
    {sizeof(cc2_sa3), {.pData = &cc2_sa3}},
    {sizeof(cc2_sa4), {.pData = &cc2_sa4}},
    {sizeof(cc2_sa5), {.pData = &cc2_sa5}},
    {sizeof(cc2_sa6), {.pData = &cc2_sa6}},
    {sizeof(cc2_sa7), {.pData = NULL}},
    {sizeof(cc2_sa8), {.pData = &cc2_sa8}},
    {sizeof(cc2_sa9), {.pData = &cc2_sa9}},
    {sizeof(cc2_sa10), {.pData = &cc2_sa10}},
};

static lcsf_valid_cmd_t cc2_msg = {
    LCSF_TEST_CMD_ID_CC2,
    cc2_att_array,
};

static uint8_t cc4_sa1 = 0;
static uint8_t cc4_ca1_sa1 = 2;
static uint16_t cc4_ca1_sa2 = 2001;
static uint32_t cc4_ca1_sa3 = 100001;
static uint8_t cc4_ca2_sa1 = 3;
static uint8_t ca2_ca3_sa1 = 4;
static uint8_t ca3_ca4_sa4[ARRAY_SIZE] = {11,21,31,41,51};

static lcsf_valid_att_t ca3_ca4_subatt_array[] = {
    {sizeof(ca3_ca4_sa4), {.pData = &ca3_ca4_sa4}},
};

static lcsf_valid_att_t ca2_ca3_subatt_array[] = {
    {0, {.pData = &ca2_ca3_sa1}},
    {0, {.pSubAttArray = ca3_ca4_subatt_array}},
};

static lcsf_valid_att_t cc4_ca2_subatt_array[] = {
    {0, {.pData = NULL}},
    {0, {.pSubAttArray = ca2_ca3_subatt_array}},
};

static lcsf_valid_att_t cc4_ca1_subatt_array[] = {
    {0, {.pData = &cc4_ca1_sa1}},
    {0, {.pData = &cc4_ca1_sa2}},
    {0, {.pData = NULL}},
};

static lcsf_valid_att_t cc4_att_array[] = {
    {0, {.pData = &cc4_sa1}},
    {0, {.pSubAttArray = cc4_ca1_subatt_array}},
    {0, {.pSubAttArray = cc4_ca2_subatt_array}},
};

static lcsf_valid_cmd_t cc4_msg = {
    LCSF_TEST_CMD_ID_CC4,
    cc4_att_array,
};

static uint16_t cc5_sa2 = 255;
static uint8_t cc5_ca5_sa1 = 1;
static uint16_t cc5_ca5_sa2 = 2000;
static uint32_t cc5_ca5_sa3 = 100000;
static uint8_t cc5_ca6_sa1 = 2;
static uint8_t ca6_ca7_sa1 = 3;
static uint8_t ca7_ca8_sa4[ARRAY_SIZE] = {10,20,30,40,50};

static lcsf_valid_att_t ca7_ca8_subatt_array[] = {
    {sizeof(ca7_ca8_sa4), {.pData = &ca7_ca8_sa4}},
};

static lcsf_valid_att_t ca6_ca7_subatt_array[] = {
    {0, {.pData = NULL}},
    {0, {.pSubAttArray = ca7_ca8_subatt_array}},
};

static lcsf_valid_att_t cc5_ca6_subatt_array[] = {
    {0, {.pData = &cc5_ca6_sa1}},
    {0, {.pSubAttArray = ca6_ca7_subatt_array}},
};

static lcsf_valid_att_t cc5_ca5_subatt_array[] = {
    {0, {.pData = &cc5_ca5_sa1}},
    {0, {.pData = &cc5_ca5_sa2}},
    {0, {.pData = NULL}},
};

static lcsf_valid_att_t cc5_att_array[] = {
    {0, {.pData = &cc5_sa2}},
    {0, {.pSubAttArray = cc5_ca5_subatt_array}},
    {0, {.pSubAttArray = cc5_ca6_subatt_array}},
};

static lcsf_valid_cmd_t cc5_msg = {
    LCSF_TEST_CMD_ID_CC5,
    cc5_att_array,
};


// *** Private Functions ***

/**
 * @brief      Compares two cc1 lcsf_valid_cmd_t
 *
 * @param[in]  p1    Pointer to first valid command
 * @param[in]  p2    Pointer to second valid command
 *
 * @return     bool: true if they are equal
 */
static bool compare_valid_cc1(const lcsf_valid_cmd_t *p1, const lcsf_valid_cmd_t *p2) {
    if ((p1 == NULL) || (p2 == NULL)) {
        LCSF_DBG_PRINT("Received a null pointer !\n");
        return false;
    }
    if ((p1->CmdId != LCSF_TEST_CMD_ID_CC1) || (p2->CmdId != LCSF_TEST_CMD_ID_CC1)) {
        LCSF_DBG_PRINT("CC1: Wrong command id: %d, %d\n", p1->CmdId, p2->CmdId);
        return false;
    }
    if ((p1->pAttArray == NULL) || (p2->pAttArray == NULL)) {
        LCSF_DBG_PRINT("CC1: Missing attributes\n");
        return false;
    }
    // SA1
    lcsf_valid_att_t *pA1 = &p1->pAttArray[0];
    lcsf_valid_att_t *pA2 = &p2->pAttArray[0];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        LCSF_DBG_PRINT("CC1: Missing SA1\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
        LCSF_DBG_PRINT("CC1: Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
        return false;
    }
    // SA2
    pA1 = &p1->pAttArray[1];
    pA2 = &p2->pAttArray[1];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        LCSF_DBG_PRINT("CC1: Missing SA2\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint16_t)) != 0) {
        LCSF_DBG_PRINT("CC1: Different SA2: %d, %d\n", *(uint16_t *)pA1->Payload.pData, *(uint16_t *)pA2->Payload.pData);
        return false;
    }
    // SA3
    pA1 = &p1->pAttArray[2];
    pA2 = &p2->pAttArray[2];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        LCSF_DBG_PRINT("CC1: Missing SA3\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint32_t)) != 0) {
        LCSF_DBG_PRINT("CC1: Different SA3: %d, %d\n", *(uint32_t *)pA1->Payload.pData, *(uint32_t *)pA2->Payload.pData);
        return false;
    }
    // SA4
    pA1 = &p1->pAttArray[3];
    pA2 = &p2->pAttArray[3];
    if (pA1->PayloadSize != pA2->PayloadSize) {
        LCSF_DBG_PRINT("CC1: Wrong SA4 size: %d, %d\n", pA1->PayloadSize, pA2->PayloadSize);
        return false;
    }
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        LCSF_DBG_PRINT("CC1: Missing SA4\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, pA1->PayloadSize) != 0) {
        LCSF_DBG_PRINT("CC1: Different SA4\n");
        return false;
    }
    // SA5
    pA1 = &p1->pAttArray[4];
    pA2 = &p2->pAttArray[4];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        LCSF_DBG_PRINT("CC1: Missing SA5\n");
        return false;
    }
    if (strcmp((char *)pA1->Payload.pData, (char *)pA2->Payload.pData) != 0) {
        LCSF_DBG_PRINT("CC1: Different SA5\n");
        return false;
    }
    // SA6
    pA1 = &p1->pAttArray[5];
    pA2 = &p2->pAttArray[5];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            LCSF_DBG_PRINT("CC1: Mismatch absence of SA6\n");
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
            LCSF_DBG_PRINT("CC1: Different SA6: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
            return false;
        }
    }
    // SA7
    pA1 = &p1->pAttArray[6];
    pA2 = &p2->pAttArray[6];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            LCSF_DBG_PRINT("CC1: Mismatch absence of SA7: %p, %p\n", pA1->Payload.pData, pA2->Payload.pData);
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint16_t)) != 0) {
            LCSF_DBG_PRINT("CC1: Different SA7: %d, %d\n", *(uint16_t *)pA1->Payload.pData, *(uint16_t *)pA2->Payload.pData);
            return false;
        }
    }
    // SA8
    pA1 = &p1->pAttArray[7];
    pA2 = &p2->pAttArray[7];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            LCSF_DBG_PRINT("CC1: Mismatch absence of SA8\n");
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint32_t)) != 0) {
            LCSF_DBG_PRINT("CC1: Different SA8: %d, %d\n", *(uint32_t *)pA1->Payload.pData, *(uint32_t *)pA2->Payload.pData);
            return false;
        }
    }
    // SA9
    pA1 = &p1->pAttArray[8];
    pA2 = &p2->pAttArray[8];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            LCSF_DBG_PRINT("CC1: Mismatch absence of SA9\n");
            return false;
        }
    } else {
        if (pA1->PayloadSize != pA2->PayloadSize) {
            LCSF_DBG_PRINT("CC1: Wrong SA9 size: %d, %d\n", pA1->PayloadSize, pA2->PayloadSize);
            return false;
        }
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, pA1->PayloadSize) != 0) {
            LCSF_DBG_PRINT("CC1: Different SA9\n");
            return false;
        }
    }
    // SA10
    pA1 = &p1->pAttArray[9];
    pA2 = &p2->pAttArray[9];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            LCSF_DBG_PRINT("CC1: Mismatch absence of SA10\n");
            return false;
        }
    } else {
        if (strcmp((char *)pA1->Payload.pData, (char *)pA2->Payload.pData) != 0) {
            LCSF_DBG_PRINT("CC1: Different SA10: %s, %s\n", (char *)pA1->Payload.pData, (char *)pA2->Payload.pData);
            return false;
        }
    }
    return true;
}

/**
 * @brief      Compares two cc2 lcsf_valid_cmd_t
 *
 * @param[in]  p1    Pointer to first valid command
 * @param[in]  p2    Pointer to second valid command
 *
 * @return     bool: true if they are equal
 */
static bool compare_valid_cc4(const lcsf_valid_cmd_t *p1, const lcsf_valid_cmd_t *p2) {
    if ((p1 == NULL) || (p2 == NULL)) {
        LCSF_DBG_PRINT("Received a null pointer !\n");
        return false;
    }
    if ((p1->CmdId != LCSF_TEST_CMD_ID_CC4) || (p2->CmdId != LCSF_TEST_CMD_ID_CC4)) {
        LCSF_DBG_PRINT("CC4: Wrong command id: %d, %d\n", p1->CmdId, p2->CmdId);
        return false;
    }
    if ((p1->pAttArray == NULL) || (p2->pAttArray == NULL)) {
        LCSF_DBG_PRINT("CC4: Missing attributes\n");
        return false;
    }
    // SA1
    lcsf_valid_att_t *pA1 = &p1->pAttArray[0];
    lcsf_valid_att_t *pA2 = &p2->pAttArray[0];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        LCSF_DBG_PRINT("CC4: Missing SA1\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
        LCSF_DBG_PRINT("CC4: Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
        return false;
    }
    // CA1
    pA1 = &p1->pAttArray[1];
    pA2 = &p2->pAttArray[1];
    if ((pA1->Payload.pSubAttArray == NULL) || (pA2->Payload.pSubAttArray == NULL)) {
        LCSF_DBG_PRINT("CC4: Missing CA1\n");
        return false;
    }
    // CA1_SA1
    pA1 = &p1->pAttArray[1].Payload.pSubAttArray[0];
    pA2 = &p2->pAttArray[1].Payload.pSubAttArray[0];
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
        LCSF_DBG_PRINT("CC4_CA1: Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
        return false;
    }
    // CA1_SA2
    pA1 = &p1->pAttArray[1].Payload.pSubAttArray[1];
    pA2 = &p2->pAttArray[1].Payload.pSubAttArray[1];
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint16_t)) != 0) {
        LCSF_DBG_PRINT("CC4_CA1: Different SA2: %d, %d\n", *(uint16_t *)pA1->Payload.pData, *(uint16_t *)pA2->Payload.pData);
        return false;
    }
    // CA1_SA3
    pA1 = &p1->pAttArray[1].Payload.pSubAttArray[2];
    pA2 = &p2->pAttArray[1].Payload.pSubAttArray[2];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            LCSF_DBG_PRINT("CC4_CA1: Mismatch absence of SA3\n");
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint32_t)) != 0) {
            LCSF_DBG_PRINT("CC4_CA1: Different SA3: %d, %d\n", *(uint32_t *)pA1->Payload.pData, *(uint32_t *)pA2->Payload.pData);
            return false;
        }
    }
    // CA2
    pA1 = &p1->pAttArray[2];
    pA2 = &p2->pAttArray[2];
    if ((pA1->Payload.pSubAttArray == NULL) || (pA2->Payload.pSubAttArray == NULL)) {
        if (pA1->Payload.pSubAttArray != pA2->Payload.pSubAttArray) {
            LCSF_DBG_PRINT("CC4: Mismatch absence of CA2\n");
            return false;
        }
    } else {
        // CA2_SA1
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[0];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[0];
        if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
            if (pA1->Payload.pData != pA2->Payload.pData) {
                LCSF_DBG_PRINT("CC4_CA2: Mismatch absence of SA1\n");
                return false;
            }
        } else {
            if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
                LCSF_DBG_PRINT("CC4_CA2: Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
                return false;
            }
        }
        // CA2_CA3
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[1];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[1];
        if ((pA1->Payload.pSubAttArray == NULL) || (pA2->Payload.pSubAttArray == NULL)) {
            LCSF_DBG_PRINT("CC4_CA2: Missing CA3\n");
            return false;
        }
        // CA3_SA1
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[0];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[0];
        if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
            if (pA1->Payload.pData != pA2->Payload.pData) {
                LCSF_DBG_PRINT("CC4_CA3: Mismatch absence of SA1\n");
                return false;
            }
        } else {
            if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
                LCSF_DBG_PRINT("CC4_CA3: Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
                return false;
            }
        }
        // CA3_CA4
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[1];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[1];
        if ((pA1->Payload.pSubAttArray == NULL) || (pA2->Payload.pSubAttArray == NULL)) {
            LCSF_DBG_PRINT("CC4_CA3: Missing CA4\n");
            return false;
        }
        // CA4_SA4
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[1].Payload.pSubAttArray[0];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[1].Payload.pSubAttArray[0];
        if (pA1->PayloadSize != pA2->PayloadSize) {
            LCSF_DBG_PRINT("CC4_CA4: Wrong SA4 size: %d, %d\n", pA1->PayloadSize, pA2->PayloadSize);
            return false;
        }
        if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
            LCSF_DBG_PRINT("CC4_CA4: Missing SA4\n");
            return false;
        }
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, pA1->PayloadSize) != 0) {
            LCSF_DBG_PRINT("CC4_CA4: Different SA4\n");
            return false;
        }
    }
    return true;
}

/**
 * @brief      Compares two test_cc2_att_payload_t
 *
 * @param[in]  p1    Pointer to first payload
 * @param[in]  p2    Pointer to second payload
 *
 * @return     bool: true if they are equal
 */
static bool compare_payload_cc2(const test_cc2_att_payload_t *p1, const test_cc2_att_payload_t *p2) {
    if (p1->optAttFlagsBitfield != p2->optAttFlagsBitfield) {
        LCSF_DBG_PRINT("CC2: Different flag bitfield: %d, %d\n", p1->optAttFlagsBitfield, p2->optAttFlagsBitfield);
        return false;
    }
    if (p1->sa1 != p2->sa1) {
        LCSF_DBG_PRINT("CC2: Different SA1: %d, %d\n", p1->sa1, p2->sa1);
        return false;
    }
    if (p1->sa2 != p2->sa2) {
        LCSF_DBG_PRINT("CC2: Different SA2: %d, %d\n", p1->sa2, p2->sa2);
        return false;
    }
    if (p1->sa3 != p2->sa3) {
        LCSF_DBG_PRINT("CC2: Different SA3: %d, %d\n", p1->sa3, p2->sa3);
        return false;
    }
    if ((p1->p_sa4 == NULL) || (p2->p_sa4 == NULL)) {
        LCSF_DBG_PRINT("CC2: Missing SA4\n");
        return false;
    }
    if (memcmp(p1->p_sa4, p2->p_sa4, ARRAY_SIZE) != 0) {
        LCSF_DBG_PRINT("CC2: Different SA4\n");
        return false;
    }
    if ((p1->p_sa5 == NULL) || (p2->p_sa5 == NULL)) {
        LCSF_DBG_PRINT("CC2: Missing SA5\n");
        return false;
    }
    if (strcmp((char *)p1->p_sa5, (char *)p2->p_sa5) != 0) {
        LCSF_DBG_PRINT("CC2: Different SA5\n");
        return false;
    }
    if ((p1->optAttFlagsBitfield & TEST_CC2_ATT_SA6_FLAG) != 0) {
        if (p1->sa6 != p2->sa6) {
            LCSF_DBG_PRINT("CC2: Different SA6: %d, %d\n", p1->sa6, p2->sa6);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC2_ATT_SA7_FLAG) != 0) {
        if (p1->sa7 != p2->sa7) {
            LCSF_DBG_PRINT("CC2: Different SA7: %d, %d\n", p1->sa7, p2->sa7);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC2_ATT_SA8_FLAG) != 0) {
        if (p1->sa8 != p2->sa8) {
            LCSF_DBG_PRINT("CC2: Different SA8: %d, %d\n", p1->sa8, p2->sa8);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC2_ATT_SA9_FLAG) != 0) {
        if ((p1->p_sa9 == NULL) || (p2->p_sa9 == NULL)) {
            LCSF_DBG_PRINT("CC2: Missing SA9\n");
            return false;
        }
        if (memcmp(p1->p_sa9, p2->p_sa9, ARRAY_SIZE) != 0) {
            LCSF_DBG_PRINT("CC2: Different SA9\n");
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC2_ATT_SA10_FLAG) != 0) {
        if ((p1->p_sa10 == NULL) || (p2->p_sa10 == NULL)) {
            LCSF_DBG_PRINT("CC2: Missing SA10\n");
            return false;
        }
        if (strcmp((char *)p1->p_sa10, (char *)p2->p_sa10) != 0) {
            LCSF_DBG_PRINT("CC2: Different SA10\n");
            return false;
        }
    }
    return true;
}

/**
 * @brief      Compares two test_cc5_att_payload_t
 *
 * @param[in]  p1    Pointer to first payload
 * @param[in]  p2    Pointer to second payload
 *
 * @return     bool: true if they are equal
 */
static bool compare_payload_cc5(const test_cc5_att_payload_t *p1, const test_cc5_att_payload_t *p2) {
    if (p1->optAttFlagsBitfield != p2->optAttFlagsBitfield) {
        LCSF_DBG_PRINT("CC5: Different flag bitfield: %d, %d\n", p1->optAttFlagsBitfield, p2->optAttFlagsBitfield);
        return false;
    }
    if (p1->sa2 != p2->sa2) {
        LCSF_DBG_PRINT("CC5: Different SA2: %d, %d\n", p1->sa2, p2->sa2);
        return false;
    }
    // CA5
    const test_cc5_att_ca5_att_payload_t *pca5_a = &p1->ca5_payload;
    const test_cc5_att_ca5_att_payload_t *pca5_b = &p2->ca5_payload;
    if (pca5_a->sa1 != pca5_b->sa1) {
        LCSF_DBG_PRINT("CC5_CA5: Different SA1: %d, %d\n", pca5_a->sa1, pca5_b->sa1);
        return false;
    }
    if (pca5_a->sa2 != pca5_b->sa2) {
        LCSF_DBG_PRINT("CC5_CA5: Different SA2: %d, %d\n", pca5_a->sa2, pca5_b->sa2);
        return false;
    }
    if ((pca5_a->optAttFlagsBitfield & TEST_CA5_ATT_SA3_FLAG) != 0) {
        if (pca5_a->sa3 != pca5_b->sa3) {
            LCSF_DBG_PRINT("CC5_CA5: Different SA3: %d, %d\n", pca5_a->sa3, pca5_b->sa3);
            return false;
        }
    }
    // CA6
    if ((p1->optAttFlagsBitfield & TEST_CC5_ATT_CA6_FLAG) != 0) {
        const test_cc5_att_ca6_att_payload_t *pca6_a = &p1->ca6_payload;
        const test_cc5_att_ca6_att_payload_t *pca6_b = &p2->ca6_payload;

        if ((pca6_a->optAttFlagsBitfield & TEST_CA6_ATT_SA1_FLAG) != 0) {
            if (pca6_a->sa1 != pca6_b->sa1) {
                LCSF_DBG_PRINT("CC5_CA6: Different SA1: %d, %d\n", pca6_a->sa1, pca6_b->sa1);
                return false;
            }
        }
        // CA7
        const test_ca6_att_ca7_att_payload_t *pca7_a = &pca6_a->ca7_payload;
        const test_ca6_att_ca7_att_payload_t *pca7_b = &pca6_b->ca7_payload;

        if ((pca7_a->optAttFlagsBitfield & TEST_CA7_ATT_SA1_FLAG) != 0) {
            if (pca7_a->sa1 != pca7_b->sa1) {
                LCSF_DBG_PRINT("CC5_CA7: Different SA1: %d, %d\n", pca7_a->sa1, pca7_b->sa1);
                return false;
            }
        }
        // CA8
        const test_ca7_att_ca8_att_payload_t *pca8_a = &pca7_a->ca8_payload;
        const test_ca7_att_ca8_att_payload_t *pca8_b = &pca7_b->ca8_payload;

        if ((pca8_a->p_sa4 == NULL) || (pca8_b->p_sa4 == NULL)) {
            LCSF_DBG_PRINT("CC5_CA8: Missing SA4\n");
            return false;
        }
        if (memcmp(pca8_a->p_sa4, pca8_b->p_sa4, pca8_a->sa4Size) != 0) {
            LCSF_DBG_PRINT("CC5_CA8: Different SA4\n");
            return false;
        }
    }
    return true;
}

/**
 * @brief      Expect a LCSF_ValidatorEncode call helper function
 *
 * @param[in]  refCmdId  Expected reference command identifier
 */
static void ExpectEncode(uint_fast16_t refCmdId) {
    mock().setData("refCmdId", (unsigned int)refCmdId);
    mock().expectOneCall("LCSF_ValidatorEncode").withParameter("protId", LCSF_TEST_PROTOCOL_ID);
}

/**
 * @brief      Expect a Test_MainExecute call helper function
 *
 * @param[in]  cmdName  Expected command name
 */
static void ExpectExecute(uint_fast16_t cmdName) {
    mock().expectOneCall("Test_MainExecute").withParameter("cmdName", cmdName);
}

// *** Public Functions ***

/**
 * @brief      mock LCSF_ValidatorEncode function
 *
 * @param[in]  protId    The prot identifier
 * @param[in]  pCommand  The command
 * @param      pBuffer   The buffer
 * @param[in]  buffSize  The buffer size
 *
 * @return     int: -1 if error, encoded message size if success
 */
int LCSF_ValidatorEncode(uint_fast16_t protId, const lcsf_valid_cmd_t *pCommand, uint8_t *pBuffer, size_t buffSize) {
    LCSF_DBG_PRINT("[tests]: Encode cmd: %d\n", pCommand->CmdId);
    mock().actualCall("LCSF_ValidatorEncode").withParameter("protId", protId);
    uint_fast16_t refCmdId = mock().getData("refCmdId").getUnsignedIntValue();
    // Process cmd id for return
    if (refCmdId != pCommand->CmdId) {
        LCSF_DBG_PRINT("[tests]: Bad cmd id\n");
        return -1;
    }
    switch (pCommand->CmdId) {
        case LCSF_TEST_CMD_ID_SC1:
            return TX_BUFF_SIZE;
        case LCSF_TEST_CMD_ID_CC1:
            if (!compare_valid_cc1(pCommand, &cc1_msg)) {
                return -1;
            }
            return TX_BUFF_SIZE;
        case LCSF_TEST_CMD_ID_CC4:
            if (!compare_valid_cc4(pCommand, &cc4_msg)) {
                return -1;
            }
            return TX_BUFF_SIZE;
        default:
            return -1;
    }
}

/**
 * @brief      mock Test_MainExecute function
 *
 * @param[in]  cmdName      The command name
 * @param      pCmdPayload  The command payload
 *
 * @return     bool: true if successful
 */
bool Test_MainExecute(uint_fast16_t cmdName, test_cmd_payload_t *pCmdPayload) {
    LCSF_DBG_PRINT("[tests]: Received send cmd: %ld\n", cmdName);
    mock().actualCall("Test_MainExecute").withParameter("cmdName", cmdName);
    // Process cmdName for return
    switch (cmdName) {
        case TEST_CMD_SC2:
            return true;
        case TEST_CMD_CC2:
            return compare_payload_cc2(&pCmdPayload->cc2_payload, &cc2_cmd_payload.cc2_payload);
        case TEST_CMD_CC5:
            return compare_payload_cc5(&pCmdPayload->cc5_payload, &cc5_cmd_payload.cc5_payload);
        default:
            return false;
    }
}

// *** Tests ***

/**
 * testgroup: A_LCSF_Bridge_Test
 *
 * This group tests all the LCSF_Bridge_Test (A) features
 */
TEST_GROUP(A_LCSF_Bridge_Test) {
    void setup() {
        mock().strictOrder();
        // Init module
        CHECK(LCSF_Bridge_TestInit());
    }
    void teardown() {
        mock().checkExpectations();
        mock().clear();
    }
};

/**
 * testgroup: A_LCSF_Bridge_Test
 * testname: execute
 *
 * Test the send/encode features
 */
TEST(A_LCSF_Bridge_Test, execute) {
    // Test error cases
    CHECK_EQUAL(LCSF_Bridge_TestEncode(TEST_CMD_CC1, NULL, txbuffer, TX_BUFF_SIZE), -1);
    // Test valid encorde
    ExpectEncode(LCSF_TEST_CMD_ID_SC1);
    CHECK_EQUAL(LCSF_Bridge_TestEncode(TEST_CMD_SC1, NULL, txbuffer, TX_BUFF_SIZE), TX_BUFF_SIZE);
    ExpectEncode(LCSF_TEST_CMD_ID_CC1);
    CHECK_EQUAL(LCSF_Bridge_TestEncode(TEST_CMD_CC1, &cc1_cmd_payload, txbuffer, TX_BUFF_SIZE), TX_BUFF_SIZE);
    ExpectEncode(LCSF_TEST_CMD_ID_CC4);
    CHECK_EQUAL(LCSF_Bridge_TestEncode(TEST_CMD_CC4, &cc4_cmd_payload, txbuffer, TX_BUFF_SIZE), TX_BUFF_SIZE);
    // Test valid receive
    ExpectExecute(TEST_CMD_SC2);
    CHECK(LCSF_Bridge_TestReceive(&sc2_msg));
    ExpectExecute(TEST_CMD_CC2);
    CHECK(LCSF_Bridge_TestReceive(&cc2_msg));
    ExpectExecute(TEST_CMD_CC5);
    CHECK(LCSF_Bridge_TestReceive(&cc5_msg));
}