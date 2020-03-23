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
#define FILO_SIZE 20
#define ARRAY_SIZE 5

// *** Private functions prototypes ***
static void *calloc_Callback(uint32_t size, int num_calls);
static void *malloc_Callback(uint32_t size, int num_calls);

// *** Private global vars ***
static void *memPtr[64];
static int memIdx;

// *** Model data ***

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
static lcsf_valid_cmd_t sc1_msg = {
    LCSF_TEST_CMD_ID_SC1,
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
    {0, {.pData = NULL}},
    {0, {.pSubAttArray = ca3_ca4_subatt_array}},
};

static lcsf_valid_att_t cc4_ca2_subatt_array[] = {
    {0, {.pData = &cc4_ca2_sa1}},
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
    {0, {.pData = &ca6_ca7_sa1}},
    {0, {.pSubAttArray = ca7_ca8_subatt_array}},
};

static lcsf_valid_att_t cc5_ca6_subatt_array[] = {
    {0, {.pData = NULL}},
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
static bool compare_valid_cc2(const lcsf_valid_cmd_t *p1, const lcsf_valid_cmd_t *p2) {
    if ((p1 == NULL) || (p2 == NULL)) {
        printf("Received a null pointer !\n");
        return false;
    }
    if ((p1->CmdId != LCSF_TEST_CMD_ID_CC2) || (p2->CmdId != LCSF_TEST_CMD_ID_CC2)) {
        printf("CC2: Wrong command id: %d, %d\n", p1->CmdId, p2->CmdId);
        return false;
    }
    if ((p1->pAttArray == NULL) || (p2->pAttArray == NULL)) {
        printf("CC2: Missing attributes\n");
        return false;
    }
    // SA1
    lcsf_valid_att_t *pA1 = &p1->pAttArray[0];
    lcsf_valid_att_t *pA2 = &p2->pAttArray[0];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("CC2: Missing SA1\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
        printf("CC2: Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
        return false;
    }
    // SA2
    pA1 = &p1->pAttArray[1];
    pA2 = &p2->pAttArray[1];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("CC2: Missing SA2\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint16_t)) != 0) {
        printf("CC2: Different SA2: %d, %d\n", *(uint16_t *)pA1->Payload.pData, *(uint16_t *)pA2->Payload.pData);
        return false;
    }
    // SA3
    pA1 = &p1->pAttArray[2];
    pA2 = &p2->pAttArray[2];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("CC2: Missing SA3\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint32_t)) != 0) {
        printf("CC2: Different SA3: %d, %d\n", *(uint32_t *)pA1->Payload.pData, *(uint32_t *)pA2->Payload.pData);
        return false;
    }
    // SA4
    pA1 = &p1->pAttArray[3];
    pA2 = &p2->pAttArray[3];
    if (pA1->PayloadSize != pA2->PayloadSize) {
        printf("CC2: Wrong SA4 size: %d, %d\n", pA1->PayloadSize, pA2->PayloadSize);
        return false;
    }
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("CC2: Missing SA4\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, pA1->PayloadSize) != 0) {
        printf("CC2: Different SA4\n");
        return false;
    }
    // SA5
    pA1 = &p1->pAttArray[4];
    pA2 = &p2->pAttArray[4];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("CC2: Missing SA5\n");
        return false;
    }
    if (strcmp(pA1->Payload.pData, pA2->Payload.pData) != 0) {
        printf("CC2: Different SA5\n");
        return false;
    }
    // SA6
    pA1 = &p1->pAttArray[5];
    pA2 = &p2->pAttArray[5];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("CC2: Mismatch absence of SA6\n");
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
            printf("CC2: Different SA6: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
            return false;
        }
    }
    // SA7
    pA1 = &p1->pAttArray[6];
    pA2 = &p2->pAttArray[6];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("CC2: Mismatch absence of SA7: %p, %p\n", pA1->Payload.pData, pA2->Payload.pData);
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint16_t)) != 0) {
            printf("CC2: Different SA7: %d, %d\n", *(uint16_t *)pA1->Payload.pData, *(uint16_t *)pA2->Payload.pData);
            return false;
        }
    }
    // SA8
    pA1 = &p1->pAttArray[7];
    pA2 = &p2->pAttArray[7];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("CC2: Mismatch absence of SA8\n");
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint32_t)) != 0) {
            printf("CC2: Different SA8: %d, %d\n", *(uint32_t *)pA1->Payload.pData, *(uint32_t *)pA2->Payload.pData);
            return false;
        }
    }
    // SA9
    pA1 = &p1->pAttArray[8];
    pA2 = &p2->pAttArray[8];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("CC2: Mismatch absence of SA9\n");
            return false;
        }
    } else {
        if (pA1->PayloadSize != pA2->PayloadSize) {
            printf("CC2: Wrong SA9 size: %d, %d\n", pA1->PayloadSize, pA2->PayloadSize);
            return false;
        }
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, pA1->PayloadSize) != 0) {
            printf("CC2: Different SA9\n");
            return false;
        }
    }
    // SA10
    pA1 = &p1->pAttArray[9];
    pA2 = &p2->pAttArray[9];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("CC2: Mismatch absence of SA10\n");
            return false;
        }
    } else {
        if (strcmp(pA1->Payload.pData, pA2->Payload.pData) != 0) {
            printf("CC2: Different SA10: %s, %s\n", (char *)pA1->Payload.pData, (char *)pA2->Payload.pData);
            return false;
        }
    }
    return true;
}

static bool compare_valid_cc5(const lcsf_valid_cmd_t *p1, const lcsf_valid_cmd_t *p2) {
    if ((p1 == NULL) || (p2 == NULL)) {
        printf("Received a null pointer !\n");
        return false;
    }
    if ((p1->CmdId != LCSF_TEST_CMD_ID_CC5) || (p2->CmdId != LCSF_TEST_CMD_ID_CC5)) {
        printf("CC5: Wrong command id: %d, %d\n", p1->CmdId, p2->CmdId);
        return false;
    }
    if ((p1->pAttArray == NULL) || (p2->pAttArray == NULL)) {
        printf("CC5: Missing attributes\n");
        return false;
    }
    // SA2
    lcsf_valid_att_t *pA1 = &p1->pAttArray[0];
    lcsf_valid_att_t *pA2 = &p2->pAttArray[0];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        printf("CC5: Missing SA2\n");
        return false;
    }
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint16_t)) != 0) {
        printf("CC5: Different SA2: %d, %d\n", *(uint16_t *)pA1->Payload.pData, *(uint16_t *)pA2->Payload.pData);
        return false;
    }
    // CA5
    pA1 = &p1->pAttArray[1];
    pA2 = &p2->pAttArray[1];
    if ((pA1->Payload.pSubAttArray == NULL) || (pA2->Payload.pSubAttArray == NULL)) {
        printf("CC5: Missing CA5\n");
        return false;
    }
    // CA5_SA1
    pA1 = &p1->pAttArray[1].Payload.pSubAttArray[0];
    pA2 = &p2->pAttArray[1].Payload.pSubAttArray[0];
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
        printf("CC5_CA5: Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
        return false;
    }
    // CA5_SA2
    pA1 = &p1->pAttArray[1].Payload.pSubAttArray[1];
    pA2 = &p2->pAttArray[1].Payload.pSubAttArray[1];
    if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint16_t)) != 0) {
        printf("CC5_CA5: Different SA2: %d, %d\n", *(uint16_t *)pA1->Payload.pData, *(uint16_t *)pA2->Payload.pData);
        return false;
    }
    // CA5_SA3
    pA1 = &p1->pAttArray[1].Payload.pSubAttArray[2];
    pA2 = &p2->pAttArray[1].Payload.pSubAttArray[2];
    if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
        if (pA1->Payload.pData != pA2->Payload.pData) {
            printf("CC5_CA3: Mismatch absence of SA3\n");
            return false;
        }
    } else {
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint32_t)) != 0) {
            printf("CC5_CA5: Different SA3: %d, %d\n", *(uint32_t *)pA1->Payload.pData, *(uint32_t *)pA2->Payload.pData);
            return false;
        }
    }
    // CA6
    pA1 = &p1->pAttArray[2];
    pA2 = &p2->pAttArray[2];
    if ((pA1->Payload.pSubAttArray == NULL) || (pA2->Payload.pSubAttArray == NULL)) {
        if (pA1->Payload.pSubAttArray != pA2->Payload.pSubAttArray) {
            printf("CC5: Mismatch absence of CA6\n");
            return false;
        }
    } else {
        // CA6_SA1
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[0];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[0];
        if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
            if (pA1->Payload.pData != pA2->Payload.pData) {
                printf("CC5_CA6: Mismatch absence of SA1\n");
                return false;
            }
        } else {
            if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
                printf("CC5_CA6: Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
                return false;
            }
        }
        // CA6_CA7
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[1];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[1];
        if ((pA1->Payload.pSubAttArray == NULL) || (pA2->Payload.pSubAttArray == NULL)) {
            printf("CC5_CA6: Missing CA7\n");
            return false;
        }
        // CA7_SA1
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[0];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[0];
        if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
            if (pA1->Payload.pData != pA2->Payload.pData) {
                printf("CC5_CA7: Mismatch absence of SA1\n");
                return false;
            }
        } else {
            if (memcmp(pA1->Payload.pData, pA2->Payload.pData, sizeof(uint8_t)) != 0) {
                printf("CC5_CA7: Different SA1: %d, %d\n", *(uint8_t *)pA1->Payload.pData, *(uint8_t *)pA2->Payload.pData);
                return false;
            }
        }
        // CA7_CA8
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[1];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[1];
        if ((pA1->Payload.pSubAttArray == NULL) || (pA2->Payload.pSubAttArray == NULL)) {
            printf("CC5_CA8: Missing CA4\n");
            return false;
        }
        // CA8_SA4
        pA1 = &p1->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[1].Payload.pSubAttArray[0];
        pA2 = &p2->pAttArray[2].Payload.pSubAttArray[1].Payload.pSubAttArray[1].Payload.pSubAttArray[0];
        if (pA1->PayloadSize != pA2->PayloadSize) {
            printf("CC5_CA8: Wrong SA4 size: %d, %d\n", pA1->PayloadSize, pA2->PayloadSize);
            return false;
        }
        if ((pA1->Payload.pData == NULL) || (pA2->Payload.pData == NULL)) {
            printf("CC5_CA8: Missing SA4\n");
            return false;
        }
        if (memcmp(pA1->Payload.pData, pA2->Payload.pData, pA1->PayloadSize) != 0) {
            printf("CC5_CA8: Different SA4\n");
            return false;
        }
    }
    return true;
}

static bool compare_payload_cc1(const test_cc1_att_payload_t *p1, const test_cc1_att_payload_t *p2) {
    if ((p1 == NULL) || (p2 == NULL)) {
        printf("Received a null pointer !\n");
        return false;
    }
    if (p1->optAttFlagsBitfield != p2->optAttFlagsBitfield) {
        printf("CC1: Different flag bitfield: %d, %d\n", p1->optAttFlagsBitfield, p2->optAttFlagsBitfield);
        return false;
    }
    if (p1->sa1 != p2->sa1) {
        printf("CC1: Different SA1: %d, %d\n", p1->sa1, p2->sa1);
        return false;
    }
    if (p1->sa2 != p2->sa2) {
        printf("CC1: Different SA2: %d, %d\n", p1->sa2, p2->sa2);
        return false;
    }
    if (p1->sa3 != p2->sa3) {
        printf("CC1: Different SA3: %d, %d\n", p1->sa3, p2->sa3);
        return false;
    }
    if ((p1->p_sa4 == NULL) || (p2->p_sa4 == NULL)) {
        printf("CC1: Missing SA4\n");
        return false;
    }
    if (memcmp(p1->p_sa4, p2->p_sa4, ARRAY_SIZE) != 0) {
        printf("CC1: Different SA4\n");
        return false;
    }
    if ((p1->p_sa5 == NULL) || (p2->p_sa5 == NULL)) {
        printf("CC1: Missing SA5\n");
        return false;
    }
    if (strcmp(p1->p_sa5, p2->p_sa5) != 0) {
        printf("CC1: Different SA5\n");
        return false;
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA6_FLAG) != 0) {
        if (p1->sa6 != p2->sa6) {
            printf("CC1: Different SA6: %d, %d\n", p1->sa6, p2->sa6);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA7_FLAG) != 0) {
        if (p1->sa7 != p2->sa7) {
            printf("CC1: Different SA7: %d, %d\n", p1->sa7, p2->sa7);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA8_FLAG) != 0) {
        if (p1->sa8 != p2->sa8) {
            printf("CC1: Different SA8: %d, %d\n", p1->sa8, p2->sa8);
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA9_FLAG) != 0) {
        if ((p1->p_sa9 == NULL) || (p2->p_sa9 == NULL)) {
            printf("CC1: Missing SA9\n");
            return false;
        }
        if (memcmp(p1->p_sa9, p2->p_sa9, ARRAY_SIZE) != 0) {
            printf("CC1: Different SA9\n");
            return false;
        }
    }
    if ((p1->optAttFlagsBitfield & TEST_CC1_ATT_SA10_FLAG) != 0) {
        if ((p1->p_sa10 == NULL) || (p2->p_sa10 == NULL)) {
            printf("CC1: Missing SA10\n");
            return false;
        }
        if (strcmp(p1->p_sa10, p2->p_sa10) != 0) {
            printf("CC1: Different SA10\n");
            return false;
        }
    }
    return true;
}

static bool compare_payload_cc4(const test_cc4_att_payload_t *p1, const test_cc4_att_payload_t *p2) {
    if ((p1 == NULL) || (p2 == NULL)) {
        printf("Received a null pointer !\n");
        return false;
    }
    if (p1->optAttFlagsBitfield != p2->optAttFlagsBitfield) {
        printf("CC4: Different flag bitfield: %d, %d\n", p1->optAttFlagsBitfield, p2->optAttFlagsBitfield);
        return false;
    }
    if (p1->sa1 != p2->sa1) {
        printf("CC4: Different SA1: %d, %d\n", p1->sa1, p2->sa1);
        return false;
    }
    // CA1
    const test_cc4_att_ca1_att_payload_t *pca1_a = &p1->ca1_payload;
    const test_cc4_att_ca1_att_payload_t *pca1_b = &p2->ca1_payload;
    if (pca1_a->sa1 != pca1_b->sa1) {
        printf("CC4_CA1: Different SA1: %d, %d\n", pca1_a->sa1, pca1_b->sa1);
        return false;
    }
    if (pca1_a->sa2 != pca1_b->sa2) {
        printf("CC4_CA1: Different SA2: %d, %d\n", pca1_a->sa2, pca1_b->sa2);
        return false;
    }
    if ((pca1_a->optAttFlagsBitfield & TEST_CA1_ATT_SA3_FLAG) != 0) {
        if (pca1_a->sa3 != pca1_b->sa3) {
            printf("CC4_CA1: Different SA3: %d, %d\n", pca1_a->sa3, pca1_b->sa3);
            return false;
        }
    }
    // CA2
    if ((p1->optAttFlagsBitfield & TEST_CC4_ATT_CA2_FLAG) != 0) {
        const test_cc4_att_ca2_att_payload_t *pca2_a = &p1->ca2_payload;
        const test_cc4_att_ca2_att_payload_t *pca2_b = &p2->ca2_payload;

        if ((pca2_a->optAttFlagsBitfield & TEST_CA2_ATT_SA1_FLAG) != 0) {
            if (pca2_a->sa1 != pca2_b->sa1) {
                printf("CC4_CA2: Different SA1: %d, %d\n", pca2_a->sa1, pca2_b->sa1);
                return false;
            }
        }
        // CA3
        const test_ca2_att_ca3_att_payload_t *pca3_a = &pca2_a->ca3_payload;
        const test_ca2_att_ca3_att_payload_t *pca3_b = &pca2_b->ca3_payload;

        if ((pca3_a->optAttFlagsBitfield & TEST_CA3_ATT_SA1_FLAG) != 0) {
            if (pca3_a->sa1 != pca3_b->sa1) {
                printf("CC4_CA3: Different SA1: %d, %d\n", pca3_a->sa1, pca3_b->sa1);
                return false;
            }
        }
        // CA4
        const test_ca3_att_ca4_att_payload_t *pca4_a = &pca3_a->ca4_payload;
        const test_ca3_att_ca4_att_payload_t *pca4_b = &pca3_b->ca4_payload;

        if ((pca4_a->p_sa4 == NULL) || (pca4_b->p_sa4 == NULL)) {
            printf("CC4_CA4: Missing SA4\n");
            return false;
        }
        if (memcmp(pca4_a->p_sa4, pca4_b->p_sa4, pca4_a->sa4Size) != 0) {
            printf("CC4_CA4: Different SA4\n");
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
    printf("Sending cmd: %d\n", pCommand->CmdId);

    if (protId != LCSF_TEST_PROTOCOL_ID) {
        printf("Wrong protocol id: %d\n", protId);
        return false;
    }
    switch (num_calls) {
        case 0:
            if ((pCommand->CmdId != LCSF_TEST_CMD_ID_SC2) || (pCommand->pAttArray != NULL)) {
                printf("Invalid SC2 valid cmd\n");
                return false;
            }
            return true;
        break;

        case 1:
            return compare_valid_cc2(pCommand, &cc2_msg);
        break;

        case 2:
            return compare_valid_cc5(pCommand, &cc5_msg);
        break;

        default:
            return false;
        break;
    }
}

static bool rx_Callback(uint16_t cmdName, test_cmd_payload_t *pCmdPayload, int num_calls) {
    printf("Received send cmd: %d\n", cmdName);

    switch (num_calls) {
        case 0:
            if (cmdName != TEST_CMD_SC1) {
                printf("Invalid SC1 cmd payload\n");
                return false;
            }
            return true;
        break;

        case 1:
            return compare_payload_cc1(&pCmdPayload->cc1_payload, &cc1_cmd_payload.cc1_payload);
        break;

        case 2:
            return compare_payload_cc4(&pCmdPayload->cc4_payload, &cc4_cmd_payload.cc4_payload);
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
    TEST_ASSERT_FALSE(LCSF_Bridge_TestSend(TEST_CMD_CC2, NULL));
    // Test valid send
    TEST_ASSERT_TRUE(LCSF_Bridge_TestSend(TEST_CMD_SC2, NULL));
    TEST_ASSERT_TRUE(LCSF_Bridge_TestSend(TEST_CMD_CC2, &cc2_cmd_payload));
    TEST_ASSERT_TRUE(LCSF_Bridge_TestSend(TEST_CMD_CC5, &cc5_cmd_payload));
    // Test valid receive
    TEST_ASSERT_TRUE(LCSF_Bridge_TestReceive(&sc1_msg));
    TEST_ASSERT_TRUE(LCSF_Bridge_TestReceive(&cc1_msg));
    TEST_ASSERT_TRUE(LCSF_Bridge_TestReceive(&cc4_msg));
}