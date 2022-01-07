#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "unity.h"

#include "LCSF_Validator.h"
#include "Filo.h"
#include "LCSF_Desc_Example.h"
#include "LCSF_Bridge_Example.h"
#include "Example_Main.h"

#include "mock_MemAlloc.h"
#include "mock_LCSF_Transcoder.h"

// *** Constants ***
#define FILO_SIZE 10
#define PROTOCOL_NB 1
#define ERROR_CODE 0xff

// *** Private functions prototypes
static bool compare_validatt(const lcsf_valid_att_t *pAtt1, const lcsf_valid_att_t *pAtt2);
static bool compare_validmsg(const lcsf_valid_cmd_t *pMsg1, const lcsf_valid_cmd_t *pMsg2);
static bool compare_rawatt(const lcsf_raw_att_t *pAtt1, const lcsf_raw_att_t *pAtt2);
static bool compare_rawmsg(const lcsf_raw_msg_t *pMsg1, const lcsf_raw_msg_t *pMsg2);
static void *malloc_Callback(size_t size, int num_calls);
static bool process_Callback(const lcsf_raw_msg_t *pMsg, int num_calls);
static bool interpret_Callback(lcsf_valid_cmd_t *pValidCmd);

// *** Descriptors ***
static const lcsf_validator_protocol_desc_t example_prot_desc = {
    LCSF_EXAMPLE_PROTOCOL_ID,
    &LCSF_Example_ProtDesc,
    interpret_Callback,
};

// *** Private global vars ***
static void *memPtr[64];
static int memIdx;

// *** Model data ***

// Model error msg
static uint8_t errloc = 0x00;
static uint8_t errcode = ERROR_CODE;

static lcsf_raw_att_t errAttArr[] = {
    {
        0x00,
        false,
        sizeof(uint8_t),
        {
            .pData = &errloc,
        }
    },
    {
        0x01,
        false,
        sizeof(uint8_t),
        {
            .pData = &errcode,
        }
    },
};

static const lcsf_raw_msg_t errMsg = {
    0xFFFF,
    0x00,
    2,
    errAttArr,
};

// Attribute data
static uint8_t att1Data[] = {0x04};

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
static bool compare_validatt(const lcsf_valid_att_t *pAtt1, const lcsf_valid_att_t *pAtt2) {
    if (pAtt1->PayloadSize != pAtt2->PayloadSize) {
        return false;
    }
    if (memcmp(pAtt1->Payload.pData, pAtt2->Payload.pData, pAtt1->PayloadSize) != 0) {
        return false;
    }
    return true;
}

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

static void *malloc_Callback(size_t size, int num_calls) {
    memPtr[memIdx] = malloc(size);
    return memPtr[memIdx++];
}

static bool process_Callback(const lcsf_raw_msg_t *pMsg, int num_calls) {
    switch (num_calls) {
        case 0:
            return compare_rawmsg(pMsg, &errMsg);
        case 1:
            return compare_rawmsg(pMsg, &rxMsg);
        default:
            return false;
    }
}

static bool interpret_Callback(lcsf_valid_cmd_t *pValidCmd) {
    return compare_validmsg(pValidCmd, &txMsg);
}

// *** Public Functions ***
void setUp(void) {
    // Set callbacks
    MemAllocMalloc_StubWithCallback(malloc_Callback);
    LCSF_TranscoderSend_StubWithCallback(process_Callback);
    // Test init module
    TEST_ASSERT_TRUE(LCSF_ValidatorInit(PROTOCOL_NB));
    TEST_ASSERT_FALSE(LCSF_ValidatorAddProtocol(0, NULL));
    TEST_ASSERT_TRUE(LCSF_ValidatorAddProtocol(0, &example_prot_desc));
}

void tearDown(void) {
    // Free allocated memory
    for (int idx = 0; idx < memIdx; idx++) {
        free(memPtr[idx]);
    }
    memIdx = 0;
}

void test_LCSF_Validator_Receive(void) {
    TEST_ASSERT_FALSE(LCSF_ValidatorReceive(NULL));
    TEST_ASSERT_TRUE(LCSF_ValidatorReceive(&rxMsg));

    // Test error receive
    uint8_t errLoc, errType;
    TEST_ASSERT_FALSE(LCSF_ValidatorTakeReceivedError(NULL, NULL));
    TEST_ASSERT_FALSE(LCSF_ValidatorTakeReceivedError(&errLoc, &errType));
    TEST_ASSERT_TRUE(LCSF_ValidatorReceive(&errMsg));
    TEST_ASSERT_TRUE(LCSF_ValidatorTakeReceivedError(&errLoc, &errType));
    TEST_ASSERT_TRUE(((errLoc == errloc) && (errType == errcode)));
}

void test_LCSF_Validator_Send(void) {
    TEST_ASSERT_TRUE(LCSF_ValidatorSendTranscoderError(ERROR_CODE));
    TEST_ASSERT_FALSE(LCSF_ValidatorSend(LCSF_EXAMPLE_PROTOCOL_ID, NULL));
    TEST_ASSERT_TRUE(LCSF_ValidatorSend(LCSF_EXAMPLE_PROTOCOL_ID, &txMsg));
}