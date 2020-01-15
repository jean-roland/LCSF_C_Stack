#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "unity.h"

#include "Filo.h"
#include "LCSF_Bridge_Test.h"
#include "mock_MemAlloc.h"
#include "mock_LCSF_Validator.h"
#include "mock_Test_Main.h"

// *** Constants ***
#define FILO_SIZE 10
#define ARRAY_SIZE 5

// *** Private functions prototypes ***
static void *calloc_Callback(uint32_t size, int num_calls);
static void *malloc_Callback(uint32_t size, int num_calls);

// *** Private global vars ***
static void *memPtr[64];
static int memIdx;

// *** Model data ***

// Model payload
uint8_t cc1_sa4_array[ARRAY_SIZE] = {6,5,4,3,2};
uint8_t cc1_sa9_array[ARRAY_SIZE] = {2,3,4,5,6};
char cc1_sa5_str[] = "Cpc";
char cc1_sa10_str[] = "Qbvm";

test_cmd_payload_t cc1_cmd_payload = {
    .cc1_payload = {
        .optAttFlagsBitfield = (TEST_CC1_ATT_SA6_FLAG | TEST_CC1_ATT_SA8_FLAG | TEST_CC1_ATT_SA9_FLAG | TEST_CC1_ATT_SA10_FLAG),
        .sa1 = 1,
        .sa2 = 2,
        .sa3 = 3,
        .sa4Size = ARRAY_SIZE,
        .p_sa4 = cc1_sa4_array,
        .p_sa5 = cc1_sa5_str,
        .sa6 = 4,
        .sa7 = 5,
        .sa8 = 6,
        .sa9Size = ARRAY_SIZE,
        .p_sa9 = cc1_sa9_array,
        .p_sa10 = cc1_sa10_str,
    },
};

uint8_t cc2_sa4_array[ARRAY_SIZE] = {5,4,3,2,1};
uint8_t cc2_sa9_array[ARRAY_SIZE] = {1,2,3,4,5};
char cc2_sa5_str[] = "Bob";
char cc2_sa10_str[] = "Paul";

test_cmd_payload_t cc2_cmd_payload = {
    .cc2_payload = {
        .optAttFlagsBitfield = (TEST_CC2_ATT_SA6_FLAG | TEST_CC2_ATT_SA8_FLAG | TEST_CC2_ATT_SA9_FLAG | TEST_CC2_ATT_SA10_FLAG),
        .sa1 = 0,
        .sa2 = 1,
        .sa3 = 2,
        .sa4Size = ARRAY_SIZE,
        .p_sa4 = cc2_sa4_array,
        .p_sa5 = cc2_sa5_str,
        .sa6 = 3,
        .sa7 = 4,
        .sa8 = 5,
        .sa9Size = ARRAY_SIZE,
        .p_sa9 = cc2_sa9_array,
        .p_sa10 = cc2_sa10_str,
    },
};

// Model valid msg
static lcsf_valid_cmd_t sc2_msg = {
    LCSF_TEST_CMD_ID_SC2,
    NULL,
};

uint8_t cc1_sa1 = 1;
uint16_t cc1_sa2 = 2;
uint32_t cc1_sa3 = 3;
uint8_t cc1_sa4[ARRAY_SIZE] = {6,5,4,3,2};
char cc1_sa5[] = "Cpc";
uint8_t cc1_sa6 = 4;
uint16_t cc1_sa7 = 5;
uint32_t cc1_sa8 = 6;
uint8_t cc1_sa9[ARRAY_SIZE] = {2,3,4,5,6};
char cc1_sa10[] = "Qbvm";

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

static const lcsf_valid_cmd_t cc1_msg = {
    LCSF_TEST_CMD_ID_CC1,
    cc1_att_array,
};

uint8_t cc2_sa1 = 0;
uint16_t cc2_sa2 = 1;
uint32_t cc2_sa3 = 2;
uint8_t cc2_sa4[ARRAY_SIZE] = {5,4,3,2,1};
char cc2_sa5[] = "Bob";
uint8_t cc2_sa6 = 3;
uint16_t cc2_sa7 = 4;
uint32_t cc2_sa8 = 5;
uint8_t cc2_sa9[ARRAY_SIZE] = {1,2,3,4,5};
char cc2_sa10[] = "Paul";

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

// *** Private Functions ***
static bool compare_valid_cc1(const lcsf_valid_cmd_t *p1, const lcsf_valid_cmd_t *p2) {
    if ((p1->CmdId != LCSF_TEST_CMD_ID_CC1) || (p2->CmdId != LCSF_TEST_CMD_ID_CC1)) {
        printf("Wrong command id: %d, %d\n", p1->CmdId, p2->CmdId);
        return false;
    }
    if ((p1->pAttArray == NULL) || (p2->pAttArray == NULL)) {
        printf("Missing attributes\n");
        return false;
    }
    // SA1
    lcsf_valid_att_t *pA1 = &p1->pAttArray[0];
    lcsf_valid_att_t *pA2 = &p2->pAttArray[0];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("Missing SA1\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
        printf("Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
        return false;
    }
    // SA2
    pA1 = &p1->pAttArray[1];
    pA2 = &p2->pAttArray[1];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("Missing SA2\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint16_t)) != 0) {
        printf("Different SA2: %d, %d\n", *(uint16_t *)pA1->Payload.pData, *(uint16_t *)pA2->Payload.pData);
        return false;
    }
    // SA3
    pA1 = &p1->pAttArray[2];
    pA2 = &p2->pAttArray[2];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("Missing SA3\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint32_t)) != 0) {
        printf("Different SA3: %d, %d\n", *(uint32_t *)pA1->Payload.pData, *(uint32_t *)pA2->Payload.pData);
        return false;
    }
    // SA4
    pA1 = &p1->pAttArray[3];
    pA2 = &p2->pAttArray[3];
    if (pA1->PayloadSize != pA2->PayloadSize) {
        printf("Wrong SA4 size: %d, %d\n", pA1->PayloadSize, pA2->PayloadSize);
        return false;
    }
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("Missing SA4\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, pA1->PayloadSize) != 0) {
        printf("Different SA4\n");
        return false;
    }
    // SA5
    pA1 = &p1->pAttArray[4];
    pA2 = &p2->pAttArray[4];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("Missing SA5\n");
        return false;
    }
    if (strcmp(pA1->Payload.pData, pA2->Payload.pData) != 0) {
        printf("Different SA5\n");
        return false;
    }
    // SA6
    pA1 = &p1->pAttArray[5];
    pA2 = &p2->pAttArray[5];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("Mismatch absence of SA6\n");
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
            printf("Different SA6: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
            return false;
        }
    }
    // SA7
    pA1 = &p1->pAttArray[6];
    pA2 = &p2->pAttArray[6];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("Mismatch absence of SA7: %p, %p\n", pA1->Payload.pData, pA2->Payload.pData);
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint16_t)) != 0) {
            printf("Different SA7: %d, %d\n", *(uint16_t *)pA1->Payload.pData, *(uint16_t *)pA2->Payload.pData);
            return false;
        }
    }
    // SA8
    pA1 = &p1->pAttArray[7];
    pA2 = &p2->pAttArray[7];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("Mismatch absence of SA8\n");
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint32_t)) != 0) {
            printf("Different SA8: %d, %d\n", *(uint32_t *)pA1->Payload.pData, *(uint32_t *)pA2->Payload.pData);
            return false;
        }
    }
    // SA9
    pA1 = &p1->pAttArray[8];
    pA2 = &p2->pAttArray[8];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("Mismatch absence of SA9\n");
            return false;
        }
    } else {
        if (pA1->PayloadSize != pA2->PayloadSize) {
            printf("Wrong SA9 size: %d, %d\n", pA1->PayloadSize, pA2->PayloadSize);
            return false;
        }
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, pA1->PayloadSize) != 0) {
            printf("Different SA9\n");
            return false;
        }
    }
    // SA10
    pA1 = &p1->pAttArray[9];
    pA2 = &p2->pAttArray[9];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("Mismatch absence of SA10\n");
            return false;
        }
    } else {
        if (strcmp(pA1->Payload.pData, pA2->Payload.pData) != 0) {
            printf("Different SA10: %s, %s\n", (char *)pA1->Payload.pData, (char *)pA2->Payload.pData);
            return false;
        }
    }
    return true;
}

static bool compare_payload_cc2(const test_cc2_att_payload_t *p1, const test_cc2_att_payload_t *p2) {
    if (p1->optAttFlagsBitfield != p2->optAttFlagsBitfield) {
        printf("Different flag bitfield: %d, %d\n", p1->optAttFlagsBitfield, p2->optAttFlagsBitfield);
        return false;
    }
    if (p1->sa1 != p2->sa1) {
        printf("Different SA1: %d, %d\n", p1->sa1, p2->sa1);
        return false;
    }
    if (p1->sa2 != p2->sa2) {
        printf("Different SA2: %d, %d\n", p1->sa2, p2->sa2);
        return false;
    }
    if (p1->sa3 != p2->sa3) {
        printf("Different SA3: %d, %d\n", p1->sa3, p2->sa3);
        return false;
    }
    if ((p1->p_sa4 == NULL) || (p2->p_sa4 == NULL)) {
        printf("Missing SA4\n");
        return false;
    }
    if (memcmp(p1->p_sa4, p2->p_sa4, ARRAY_SIZE) != 0) {
        printf("Different SA4\n");
        return false;
    }
    if ((p1->p_sa5 == NULL) || (p2->p_sa5 == NULL)) {
        printf("Missing SA5\n");
        return false;
    }
    if (strcmp(p1->p_sa5, p2->p_sa5) != 0) {
        printf("Different SA5\n");
        return false;
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA6_FLAG) != 0) {
        if (p1->sa6 != p2->sa6) {
            printf("Different SA6: %d, %d\n", p1->sa6, p2->sa6);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA7_FLAG) != 0) {
        if (p1->sa7 != p2->sa7) {
            printf("Different SA7: %d, %d\n", p1->sa7, p2->sa7);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA8_FLAG) != 0) {
        if (p1->sa8 != p2->sa8) {
            printf("Different SA8: %d, %d\n", p1->sa8, p2->sa8);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA9_FLAG) != 0) {
        if ((p1->p_sa9 == NULL) || (p2->p_sa9 == NULL)) {
            printf("Missing SA9\n");
            return false;
        }
        if (memcmp(p1->p_sa9, p2->p_sa9, ARRAY_SIZE) != 0) {
            printf("Different SA9\n");
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA10_FLAG) != 0) {
        if ((p1->p_sa10 == NULL) || (p2->p_sa10 == NULL)) {
            printf("Missing SA10\n");
            return false;
        }
        if (strcmp(p1->p_sa10, p2->p_sa10) != 0) {
            printf("Different SA10\n");
            return false;
        }
    }
    return true;
}

// *** Callback Functions ***
static void *calloc_Callback(uint32_t size, int num_calls) {
    memPtr[memIdx] = calloc(size,1);
    return memPtr[memIdx++];
}

static void *malloc_Callback(uint32_t size, int num_calls) {
    memPtr[memIdx] = malloc(size);
    return memPtr[memIdx++];
}

static bool tx_Callback(uint8_t protId, const lcsf_valid_cmd_t *pCommand, int num_calls) {
    if (protId != LCSF_TEST_PROTOCOL_ID) {
        printf("Wrong protocol id: %d\n", protId);
        return false;
    }
    switch (num_calls) {
        case 0:
            if ((pCommand->CmdId != LCSF_TEST_CMD_ID_SC1) || (pCommand->pAttArray != NULL)) {
                printf("Invalid SC1 valid cmd\n");
                return false;
            }
            return true;
        break;

        case 1:
            return compare_valid_cc1(pCommand, &cc1_msg);
        break;

        case 2:
            // CC4
            return false;
        break;

        default:
            return false;
        break;
    }
}

static bool rx_Callback(uint16_t cmdName, test_cmd_payload_t *pCmdPayload, int num_calls) {
    switch (num_calls) {
        case 0:
            if (cmdName != TEST_CMD_SC2) {
                printf("Invalid SC2 cmd payload\n");
                return false;
            }
            return true;
        break;

        case 1:
            return compare_payload_cc2(&pCmdPayload->cc2_payload, &cc2_cmd_payload.cc2_payload);
        break;

        case 2:
            // CC5
            return false;
        break;

        default:
            return false;
        break;
    }
}

// *** Public Functions ***
void setUp(void) {
    // Declare callback
    MemAllocCalloc_StubWithCallback(calloc_Callback);
    MemAllocMalloc_StubWithCallback(malloc_Callback);
    LCSF_ValidatorSend_StubWithCallback(tx_Callback);
    Test_MainExecute_StubWithCallback(rx_Callback);
    // Test init module
    TEST_ASSERT_TRUE(LCSF_Bridge_TestInit(FILO_SIZE));
}

void tearDown(void) {
    // Free allocated memory
    for (int idx = 0; idx < memIdx; idx++) {
        free(memPtr[idx]);
    }
    memIdx = 0;
}

void test_Test_Main_Execute(void) {
    // Test error cases
    TEST_ASSERT_FALSE(LCSF_Bridge_TestSend(TEST_CMD_CC1, NULL));
    // Test valid send
    TEST_ASSERT_TRUE(LCSF_Bridge_TestSend(TEST_CMD_SC1, NULL));
    TEST_ASSERT_TRUE(LCSF_Bridge_TestSend(TEST_CMD_CC1, &cc1_cmd_payload));
    // Test valid receive
    TEST_ASSERT_TRUE(LCSF_Bridge_TestReceive(&sc2_msg));
    TEST_ASSERT_TRUE(LCSF_Bridge_TestReceive(&cc2_msg));
}