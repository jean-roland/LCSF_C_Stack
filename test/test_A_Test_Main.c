#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "unity.h"

#include "Test_Main.h"
#include "mock_MemAlloc.h"
#include "mock_LCSF_Bridge_Test.h"

// *** Constants ***
#define ARRAY_SIZE 5

// *** Private functions prototypes ***
static void *calloc_Callback(uint32_t size, int num_calls);

// *** Private global vars ***
static void *memPtr[64];
static int memIdx;

// *** Model data ***
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
    if (p1->sa4Size != p2->sa4Size) {
        printf("CC1: Different SA4 size: %d, %d\n", p1->sa4Size, p2->sa4Size);
        return false;
    }
    if ((p1->p_sa4 == NULL) || (p2->p_sa4 == NULL)) {
        printf("CC1: Missing SA4\n");
        return false;
    }
    if (memcmp(p1->p_sa4, p2->p_sa4, p1->sa4Size) != 0) {
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
        if (p1->sa9Size != p2->sa9Size) {
            printf("CC1: Different SA9 size: %d, %d\n", p1->sa9Size, p2->sa9Size);
            return false;
        }
        if ((p1->p_sa9 == NULL) || (p2->p_sa9 == NULL)) {
            printf("CC1: Missing SA9\n");
            return false;
        }
        if (memcmp(p1->p_sa9, p2->p_sa9, p1->sa9Size) != 0) {
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

        if (pca4_a->sa4Size != pca4_b->sa4Size) {
            printf("CC4_CA4: Different SA4 size: %d, %d\n", pca4_a->sa4Size, pca4_b->sa4Size);
            return false;
        }
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

static bool process_Callback(uint16_t cmdName, test_cmd_payload_t *pCmdPayload, int num_calls) {
    printf("Received send cmd: %d\n", cmdName);

    switch (num_calls) {
        case 0:
            return (cmdName == TEST_CMD_SC1);
        break;

        case 1:
            return compare_payload_cc1(&cc1_cmd_payload.cc1_payload, &pCmdPayload->cc1_payload);
        break;

        case 2:
            return compare_payload_cc4(&cc4_cmd_payload.cc4_payload, &pCmdPayload->cc4_payload);
        break;

        case 3: {
            test_cc1_att_payload_t *p = &pCmdPayload->cc1_payload;
            return ((memcmp(p->p_sa4, pattern, ARRAY_SIZE) == 0) && (memcmp(p->p_sa9, pattern, ARRAY_SIZE) == 0));
        break; }

        default:
            return false;
        break;
    }
}

// *** Public Functions ***
void setUp(void) {
    // Declare callback
    MemAllocCalloc_StubWithCallback(calloc_Callback);
    LCSF_Bridge_TestSend_StubWithCallback(process_Callback);
    // Test init module
    TEST_ASSERT_TRUE(Test_MainInit());
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
    TEST_ASSERT_FALSE(Test_MainExecute(TEST_CMD_COUNT, NULL));
    TEST_ASSERT_FALSE(Test_MainExecute(TEST_CMD_CC2, NULL));
    // Test valid cases
    TEST_ASSERT_TRUE(Test_MainExecute(TEST_CMD_SC2, NULL));
    TEST_ASSERT_TRUE(Test_MainExecute(TEST_CMD_CC2, &cc2_cmd_payload));
    TEST_ASSERT_TRUE(Test_MainExecute(TEST_CMD_CC5, &cc5_cmd_payload));
    // Test pattern
    Test_MainSetPattern(pattern);
    TEST_ASSERT_TRUE(Test_MainExecute(TEST_CMD_CC2, &cc2_cmd_payload));
}