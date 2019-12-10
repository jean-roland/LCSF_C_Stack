#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "unity.h"

#include "LCSF_Transcoder.h"
#include "Filo.h"

#include "mock_MemAlloc.h"
#include "mock_LCSF_Validator.h"

// *** Constants ***
#define FILO_SIZE 10
#define BUFF_SIZE 255

// *** Private functions prototypes
static bool compare_rawatt(const lcsf_raw_att_t *pAtt1, const lcsf_raw_att_t *pAtt2);
static bool compare_rawmsg(const lcsf_raw_msg_t *pMsg1, const lcsf_raw_msg_t *pMsg2);
static void *calloc_Callback(uint32_t size, int num_calls);
static void *malloc_Callback(uint32_t size, int num_calls);
static bool process_error_Callback(uint8_t error_code, int num_calls);
static bool process_msg_Callback(const lcsf_raw_msg_t *pMsg, int num_calls);
static bool send_Callback(const uint8_t *pBuffer, uint16_t buffSize);

// *** Descriptors ***
static const lcsf_trnscdr_init_desc_t init_trnscdr = {
    send_Callback,
    FILO_SIZE,
    BUFF_SIZE,
};

// *** Private global vars ***
static void *memPtr[64];
static int memIdx;

// *** Model data ***
#ifdef LCSF_SMALL

static const uint8_t ovrflwMsg[] = {0xaa, 0x01, 0x0b};

static const uint8_t badformatMsg[] = {0xaa, 0x01, 0x0a};

static const uint8_t rxMsg[] = {
0xab, 0x12, 0x03, 0x55, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04,
0xff, 0x02, 0x30, 0x01, 0x0a, 0xb1, 0x01, 0x32, 0x02, 0xab,
0xcd, 0x40, 0x10, 0x4f, 0x72, 0x67, 0x61, 0x6e, 0x6f, 0x6c,
0x65, 0x70, 0x74, 0x69, 0x63, 0x00, 0x00, 0x00, 0x00
};

// Model raw msg
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
        0x7f,
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
    0x12,
    sizeof(txAttArr)/sizeof(lcsf_raw_att_t),
    txAttArr
};

#else

static const uint8_t ovrflwMsg[] = {0xaa, 0x00, 0x01, 0x00, 0x0b, 0x00};

static const uint8_t badformatMsg[] = {0xaa, 0x00, 0x01, 0x00, 0x0a, 0x00};

static const uint8_t rxMsg[] = {
0xab, 0x00, 0x00, 0x12, 0x03, 0x00, 0x55, 0x00, 0x05, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04,
0xff, 0xff, 0x02, 0x00, 0x30, 0x00, 0x01, 0x00, 0x0a, 0x31, 0x80, 0x01, 0x00, 0x32, 0x00,
0x02, 0x00, 0xab, 0xcd, 0x40, 0x00, 0x10, 0x00, 0x4f, 0x72, 0x67, 0x61, 0x6e, 0x6f, 0x6c,
0x65, 0x70, 0x74, 0x69, 0x63, 0x00, 0x00, 0x00, 0x00
};

// Model raw msg
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
        0x7fff,
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
    0x1200,
    sizeof(txAttArr)/sizeof(lcsf_raw_att_t),
    txAttArr
};
#endif

// *** Private Functions ***
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

static void *calloc_Callback(uint32_t size, int num_calls) {
    memPtr[memIdx] = calloc(size,1);
    return memPtr[memIdx++];
}

static void *malloc_Callback(uint32_t size, int num_calls) {
    memPtr[memIdx] = malloc(size);
    return memPtr[memIdx++];
}

static bool process_error_Callback(uint8_t error_code, int num_calls) {
    switch (num_calls) {
        case 0:
            return (error_code == 0xFF);
        case 1:
            return (error_code == 0x01);
        case 2:
            return (error_code == 0x00);
        default:
            return false;
    }
}

static bool process_msg_Callback(const lcsf_raw_msg_t *pMsg, int num_calls) {
    return compare_rawmsg(pMsg, &txMsg);
}

static bool send_Callback(const uint8_t *pBuffer, uint16_t buffSize) {
    return (memcmp(pBuffer, rxMsg, buffSize) == 0);
}

// *** Public Functions ***
void setUp(void) {
    // Declare callback
    MemAllocCalloc_StubWithCallback(calloc_Callback);
    MemAllocMalloc_StubWithCallback(malloc_Callback);
    LCSF_ValidatorSendTranscoderError_StubWithCallback(process_error_Callback);
    LCSF_ValidatorReceive_StubWithCallback(process_msg_Callback);
    // Test init module
    TEST_ASSERT_FALSE(LCSF_TranscoderInit(NULL));
    TEST_ASSERT_TRUE(LCSF_TranscoderInit(&init_trnscdr));
}

void tearDown(void) {
    // Free allocated memory
    for (int idx = 0; idx < memIdx; idx++) {
        free(memPtr[idx]);
    }
    memIdx = 0;
}

void test_LCSF_Transcoder_Encode(void) {
#ifdef LCSF_SMALL
    printf("Smaller LCSF representation is in use.\n");
#else
    printf("Regular LCSF representation is in use.\n");
#endif
    // Test error cases
    TEST_ASSERT_FALSE(LCSF_TranscoderSend(NULL));
    // Test valid message
    TEST_ASSERT_TRUE(LCSF_TranscoderSend(&txMsg));
}

void test_LCSF_Transcoder_Decode(void) {
    // Test error cases
    TEST_ASSERT_TRUE(LCSF_TranscoderReceive(NULL, 0));
    TEST_ASSERT_TRUE(LCSF_TranscoderReceive(ovrflwMsg, sizeof(ovrflwMsg)));
    TEST_ASSERT_TRUE(LCSF_TranscoderReceive(badformatMsg, sizeof(badformatMsg)));
    // Test valid packet
    TEST_ASSERT_TRUE(LCSF_TranscoderReceive(rxMsg, sizeof(rxMsg)));
}
