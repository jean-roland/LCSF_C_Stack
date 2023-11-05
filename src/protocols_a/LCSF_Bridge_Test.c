/**
 * \file LCSF_Bridge_Test.c
 * \brief Test LCSF bridge module (A)
 * \author LCSF Generator v1.4
 *
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include "LCSF_Bridge_Test.h"
#include <Filo.h>
#include <LCSF_Config.h>
#include <LCSF_Transcoder.h>
#include <LCSF_Validator.h>

// *** Definitions ***
// --- Private Types ---

// Module information structure
typedef struct _lcsf_bridge_test_info {
    uint8_t FiloData[LCSF_BRIDGE_TEST_FILO_SIZE * sizeof(lcsf_valid_att_t)];
    filo_desc_t Filo;
    test_cmd_payload_t CmdPayload;
} lcsf_bridge_test_info_t;

// --- Private Constants ---
// Array to convert command name value to their lcsf command id
static const uint16_t LCSF_Bridge_Test_CMDNAME2CMDID[LCSF_TEST_CMD_NB] = {
    LCSF_TEST_CMD_ID_SC1,
    LCSF_TEST_CMD_ID_SC2,
    LCSF_TEST_CMD_ID_SC3,
    LCSF_TEST_CMD_ID_CC1,
    LCSF_TEST_CMD_ID_CC2,
    LCSF_TEST_CMD_ID_CC3,
    LCSF_TEST_CMD_ID_CC4,
    LCSF_TEST_CMD_ID_CC5,
    LCSF_TEST_CMD_ID_CC6,
};

// --- Private Function Prototypes ---
static uint16_t LCSF_Bridge_Test_CMDID2CMDNAME(uint_fast16_t cmdId);
static void LCSF_Bridge_TestCC2GetData(lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload);
static void LCSF_Bridge_TestCC3GetData(lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload);
static void LCSF_Bridge_TestCC5GetData(lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload);
static void LCSF_Bridge_TestCC6GetData(lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload);
static void LCSF_Bridge_TestGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_TestCC1FillAtt(lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_TestCC3FillAtt(lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_TestCC4FillAtt(lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_TestCC6FillAtt(lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_TestFillCmdAtt(
    uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static lcsf_bridge_test_info_t LcsfBridgeTestInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static uint16_t LCSF_Bridge_Test_CMDID2CMDNAME(uint_fast16_t cmdId)
 * \brief Translate an lcsf command id to its name value
 *
 * \param cmdId lcsf command identifier to translate
 * \return uint16_t: name value of the command
 */
static uint16_t LCSF_Bridge_Test_CMDID2CMDNAME(uint_fast16_t cmdId) {
    switch (cmdId) {
        default:
        case LCSF_TEST_CMD_ID_SC1:
            return TEST_CMD_SC1;
        case LCSF_TEST_CMD_ID_SC2:
            return TEST_CMD_SC2;
        case LCSF_TEST_CMD_ID_SC3:
            return TEST_CMD_SC3;
        case LCSF_TEST_CMD_ID_CC1:
            return TEST_CMD_CC1;
        case LCSF_TEST_CMD_ID_CC2:
            return TEST_CMD_CC2;
        case LCSF_TEST_CMD_ID_CC3:
            return TEST_CMD_CC3;
        case LCSF_TEST_CMD_ID_CC4:
            return TEST_CMD_CC4;
        case LCSF_TEST_CMD_ID_CC5:
            return TEST_CMD_CC5;
        case LCSF_TEST_CMD_ID_CC6:
            return TEST_CMD_CC6;
    }
}

/**
 * \fn static void LCSF_Bridge_TestXGetData(lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload)
 * \brief Retrieve data of command X from its valid attribute array and store it in a payload
 *
 * \param pAttArray pointer to the command attribute array
 * \param pCmdPayload pointer to the payload to contain the command data
 * \return void
 */
static void LCSF_Bridge_TestCC2GetData(lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return;
    }
    // Initialize optional attribute flags bitfield
    pCmdPayload->cc2_payload.optAttFlagsBitfield = 0;
    // Retrieve data of attribute SA1
    memcpy(
        &(pCmdPayload->cc2_payload.sa1), pAttArray[TEST_CC2_ATT_SA1].Payload.pData, pAttArray[TEST_CC2_ATT_SA1].PayloadSize);
    // Retrieve data of attribute SA2
    memcpy(
        &(pCmdPayload->cc2_payload.sa2), pAttArray[TEST_CC2_ATT_SA2].Payload.pData, pAttArray[TEST_CC2_ATT_SA2].PayloadSize);
    // Retrieve data of attribute SA3
    memcpy(
        &(pCmdPayload->cc2_payload.sa3), pAttArray[TEST_CC2_ATT_SA3].Payload.pData, pAttArray[TEST_CC2_ATT_SA3].PayloadSize);
    // Retrieve data of attribute SA4
    pCmdPayload->cc2_payload.p_sa4 = pAttArray[TEST_CC2_ATT_SA4].Payload.pData;
    // Retrieve data of attribute SA5
    pCmdPayload->cc2_payload.p_sa5 = pAttArray[TEST_CC2_ATT_SA5].Payload.pData;
    // Retrieve data of attribute SA6
    if (pAttArray[TEST_CC2_ATT_SA6].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA6_FLAG;
        memcpy(&(pCmdPayload->cc2_payload.sa6), pAttArray[TEST_CC2_ATT_SA6].Payload.pData,
            pAttArray[TEST_CC2_ATT_SA6].PayloadSize);
    }
    // Retrieve data of attribute SA7
    if (pAttArray[TEST_CC2_ATT_SA7].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA7_FLAG;
        memcpy(&(pCmdPayload->cc2_payload.sa7), pAttArray[TEST_CC2_ATT_SA7].Payload.pData,
            pAttArray[TEST_CC2_ATT_SA7].PayloadSize);
    }
    // Retrieve data of attribute SA8
    if (pAttArray[TEST_CC2_ATT_SA8].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA8_FLAG;
        memcpy(&(pCmdPayload->cc2_payload.sa8), pAttArray[TEST_CC2_ATT_SA8].Payload.pData,
            pAttArray[TEST_CC2_ATT_SA8].PayloadSize);
    }
    // Retrieve data of attribute SA9
    if (pAttArray[TEST_CC2_ATT_SA9].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA9_FLAG;
        pCmdPayload->cc2_payload.p_sa9 = pAttArray[TEST_CC2_ATT_SA9].Payload.pData;
    }
    // Retrieve data of attribute SA10
    if (pAttArray[TEST_CC2_ATT_SA10].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA10_FLAG;
        pCmdPayload->cc2_payload.p_sa10 = pAttArray[TEST_CC2_ATT_SA10].Payload.pData;
    }
    // Retrieve data of attribute SA11
    memcpy(&(pCmdPayload->cc2_payload.sa11), pAttArray[TEST_CC2_ATT_SA11].Payload.pData,
        pAttArray[TEST_CC2_ATT_SA11].PayloadSize);
    // Retrieve data of attribute SA12
    memcpy(&(pCmdPayload->cc2_payload.sa12), pAttArray[TEST_CC2_ATT_SA12].Payload.pData,
        pAttArray[TEST_CC2_ATT_SA12].PayloadSize);
    // Retrieve data of attribute SA13
    memcpy(&(pCmdPayload->cc2_payload.sa13), pAttArray[TEST_CC2_ATT_SA13].Payload.pData,
        pAttArray[TEST_CC2_ATT_SA13].PayloadSize);
}

static void LCSF_Bridge_TestCC3GetData(lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return;
    }
    // Initialize optional attribute flags bitfield
    pCmdPayload->cc3_payload.optAttFlagsBitfield = 0;
    // Retrieve data of attribute SA1
    memcpy(
        &(pCmdPayload->cc3_payload.sa1), pAttArray[TEST_CC3_ATT_SA1].Payload.pData, pAttArray[TEST_CC3_ATT_SA1].PayloadSize);
    // Retrieve data of attribute SA2
    memcpy(
        &(pCmdPayload->cc3_payload.sa2), pAttArray[TEST_CC3_ATT_SA2].Payload.pData, pAttArray[TEST_CC3_ATT_SA2].PayloadSize);
    // Retrieve data of attribute SA3
    memcpy(
        &(pCmdPayload->cc3_payload.sa3), pAttArray[TEST_CC3_ATT_SA3].Payload.pData, pAttArray[TEST_CC3_ATT_SA3].PayloadSize);
    // Retrieve data of attribute SA4
    pCmdPayload->cc3_payload.p_sa4 = pAttArray[TEST_CC3_ATT_SA4].Payload.pData;
    // Retrieve data of attribute SA5
    pCmdPayload->cc3_payload.p_sa5 = pAttArray[TEST_CC3_ATT_SA5].Payload.pData;
    // Retrieve data of attribute SA6
    if (pAttArray[TEST_CC3_ATT_SA6].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA6_FLAG;
        memcpy(&(pCmdPayload->cc3_payload.sa6), pAttArray[TEST_CC3_ATT_SA6].Payload.pData,
            pAttArray[TEST_CC3_ATT_SA6].PayloadSize);
    }
    // Retrieve data of attribute SA7
    if (pAttArray[TEST_CC3_ATT_SA7].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA7_FLAG;
        memcpy(&(pCmdPayload->cc3_payload.sa7), pAttArray[TEST_CC3_ATT_SA7].Payload.pData,
            pAttArray[TEST_CC3_ATT_SA7].PayloadSize);
    }
    // Retrieve data of attribute SA8
    if (pAttArray[TEST_CC3_ATT_SA8].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA8_FLAG;
        memcpy(&(pCmdPayload->cc3_payload.sa8), pAttArray[TEST_CC3_ATT_SA8].Payload.pData,
            pAttArray[TEST_CC3_ATT_SA8].PayloadSize);
    }
    // Retrieve data of attribute SA9
    if (pAttArray[TEST_CC3_ATT_SA9].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA9_FLAG;
        pCmdPayload->cc3_payload.p_sa9 = pAttArray[TEST_CC3_ATT_SA9].Payload.pData;
    }
    // Retrieve data of attribute SA10
    if (pAttArray[TEST_CC3_ATT_SA10].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA10_FLAG;
        pCmdPayload->cc3_payload.p_sa10 = pAttArray[TEST_CC3_ATT_SA10].Payload.pData;
    }
    // Retrieve data of attribute SA11
    memcpy(&(pCmdPayload->cc3_payload.sa11), pAttArray[TEST_CC3_ATT_SA11].Payload.pData,
        pAttArray[TEST_CC3_ATT_SA11].PayloadSize);
    // Retrieve data of attribute SA12
    memcpy(&(pCmdPayload->cc3_payload.sa12), pAttArray[TEST_CC3_ATT_SA12].Payload.pData,
        pAttArray[TEST_CC3_ATT_SA12].PayloadSize);
    // Retrieve data of attribute SA13
    memcpy(&(pCmdPayload->cc3_payload.sa13), pAttArray[TEST_CC3_ATT_SA13].Payload.pData,
        pAttArray[TEST_CC3_ATT_SA13].PayloadSize);
}

static void LCSF_Bridge_TestCC5GetData(lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return;
    }
    // Initialize optional attribute flags bitfield
    pCmdPayload->cc5_payload.optAttFlagsBitfield = 0;
    // Retrieve data of attribute SA2
    memcpy(
        &(pCmdPayload->cc5_payload.sa2), pAttArray[TEST_CC5_ATT_SA2].Payload.pData, pAttArray[TEST_CC5_ATT_SA2].PayloadSize);
    // Retrieve data of attribute CA5
    // Initialize optional attribute flags bitfield
    pCmdPayload->cc5_payload.ca5_payload.optAttFlagsBitfield = 0;
    // Retrieve data of sub-attribute SA1
    memcpy(&(pCmdPayload->cc5_payload.ca5_payload.sa1),
        pAttArray[TEST_CC5_ATT_CA5].Payload.pSubAttArray[TEST_CA5_ATT_SA1].Payload.pData,
        pAttArray[TEST_CC5_ATT_CA5].Payload.pSubAttArray[TEST_CA5_ATT_SA1].PayloadSize);
    // Retrieve data of sub-attribute SA2
    memcpy(&(pCmdPayload->cc5_payload.ca5_payload.sa2),
        pAttArray[TEST_CC5_ATT_CA5].Payload.pSubAttArray[TEST_CA5_ATT_SA2].Payload.pData,
        pAttArray[TEST_CC5_ATT_CA5].Payload.pSubAttArray[TEST_CA5_ATT_SA2].PayloadSize);
    // Retrieve data of sub-attribute SA3
    if (pAttArray[TEST_CC5_ATT_CA5].Payload.pSubAttArray[TEST_CA5_ATT_SA3].Payload.pData != NULL) {
        // Note presence of optional attribute SA3
        pCmdPayload->cc5_payload.ca5_payload.optAttFlagsBitfield |= TEST_CA5_ATT_SA3_FLAG;
        // Retrieve data of sub-attribute SA3
        memcpy(&(pCmdPayload->cc5_payload.ca5_payload.sa3),
            pAttArray[TEST_CC5_ATT_CA5].Payload.pSubAttArray[TEST_CA5_ATT_SA3].Payload.pData,
            pAttArray[TEST_CC5_ATT_CA5].Payload.pSubAttArray[TEST_CA5_ATT_SA3].PayloadSize);
    }
    // Retrieve data of attribute CA6
    if (pAttArray[TEST_CC5_ATT_CA6].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc5_payload.optAttFlagsBitfield |= TEST_CC5_ATT_CA6_FLAG;
        // Initialize optional attribute flags bitfield
        pCmdPayload->cc5_payload.ca6_payload.optAttFlagsBitfield = 0;
        // Retrieve data of sub-attribute SA1
        if (pAttArray[TEST_CC5_ATT_CA6].Payload.pSubAttArray[TEST_CA6_ATT_SA1].Payload.pData != NULL) {
            // Note presence of optional attribute SA1
            pCmdPayload->cc5_payload.ca6_payload.optAttFlagsBitfield |= TEST_CA6_ATT_SA1_FLAG;
            // Retrieve data of sub-attribute SA1
            memcpy(&(pCmdPayload->cc5_payload.ca6_payload.sa1),
                pAttArray[TEST_CC5_ATT_CA6].Payload.pSubAttArray[TEST_CA6_ATT_SA1].Payload.pData,
                pAttArray[TEST_CC5_ATT_CA6].Payload.pSubAttArray[TEST_CA6_ATT_SA1].PayloadSize);
        }
        // Retrieve data of sub-attribute CA7
        // Initialize optional attribute flags bitfield
        pCmdPayload->cc5_payload.ca6_payload.ca7_payload.optAttFlagsBitfield = 0;
        // Retrieve data of sub-attribute SA1
        if (pAttArray[TEST_CC5_ATT_CA6]
                .Payload.pSubAttArray[TEST_CA6_ATT_CA7]
                .Payload.pSubAttArray[TEST_CA7_ATT_SA1]
                .Payload.pData != NULL) {
            // Note presence of optional attribute SA1
            pCmdPayload->cc5_payload.ca6_payload.ca7_payload.optAttFlagsBitfield |= TEST_CA7_ATT_SA1_FLAG;
            // Retrieve data of sub-attribute SA1
            memcpy(&(pCmdPayload->cc5_payload.ca6_payload.ca7_payload.sa1),
                pAttArray[TEST_CC5_ATT_CA6]
                    .Payload.pSubAttArray[TEST_CA6_ATT_CA7]
                    .Payload.pSubAttArray[TEST_CA7_ATT_SA1]
                    .Payload.pData,
                pAttArray[TEST_CC5_ATT_CA6]
                    .Payload.pSubAttArray[TEST_CA6_ATT_CA7]
                    .Payload.pSubAttArray[TEST_CA7_ATT_SA1]
                    .PayloadSize);
        }
        // Retrieve data of sub-attribute CA8
        // Retrieve data of sub-attribute SA4
        pCmdPayload->cc5_payload.ca6_payload.ca7_payload.ca8_payload.p_sa4 = pAttArray[TEST_CC5_ATT_CA6]
                                                                                 .Payload.pSubAttArray[TEST_CA6_ATT_CA7]
                                                                                 .Payload.pSubAttArray[TEST_CA7_ATT_CA8]
                                                                                 .Payload.pSubAttArray[TEST_CA8_ATT_SA4]
                                                                                 .Payload.pData;
    }
}

static void LCSF_Bridge_TestCC6GetData(lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return;
    }
    // Initialize optional attribute flags bitfield
    pCmdPayload->cc6_payload.optAttFlagsBitfield = 0;
    // Retrieve data of attribute SA4
    pCmdPayload->cc6_payload.p_sa4 = pAttArray[TEST_CC6_ATT_SA4].Payload.pData;
    // Retrieve data of attribute CA9
    // Initialize optional attribute flags bitfield
    pCmdPayload->cc6_payload.ca9_payload.optAttFlagsBitfield = 0;
    // Retrieve data of sub-attribute SA1
    memcpy(&(pCmdPayload->cc6_payload.ca9_payload.sa1),
        pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA1].Payload.pData,
        pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA1].PayloadSize);
    // Retrieve data of sub-attribute SA2
    memcpy(&(pCmdPayload->cc6_payload.ca9_payload.sa2),
        pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA2].Payload.pData,
        pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA2].PayloadSize);
    // Retrieve data of sub-attribute SA3
    if (pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA3].Payload.pData != NULL) {
        // Note presence of optional attribute SA3
        pCmdPayload->cc6_payload.ca9_payload.optAttFlagsBitfield |= TEST_CA9_ATT_SA3_FLAG;
        // Retrieve data of sub-attribute SA3
        memcpy(&(pCmdPayload->cc6_payload.ca9_payload.sa3),
            pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA3].Payload.pData,
            pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA3].PayloadSize);
    }
    // Retrieve data of attribute CA10
    if (pAttArray[TEST_CC6_ATT_CA10].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->cc6_payload.optAttFlagsBitfield |= TEST_CC6_ATT_CA10_FLAG;
        // Initialize optional attribute flags bitfield
        pCmdPayload->cc6_payload.ca10_payload.optAttFlagsBitfield = 0;
        // Retrieve data of sub-attribute SA1
        if (pAttArray[TEST_CC6_ATT_CA10].Payload.pSubAttArray[TEST_CA10_ATT_SA1].Payload.pData != NULL) {
            // Note presence of optional attribute SA1
            pCmdPayload->cc6_payload.ca10_payload.optAttFlagsBitfield |= TEST_CA10_ATT_SA1_FLAG;
            // Retrieve data of sub-attribute SA1
            memcpy(&(pCmdPayload->cc6_payload.ca10_payload.sa1),
                pAttArray[TEST_CC6_ATT_CA10].Payload.pSubAttArray[TEST_CA10_ATT_SA1].Payload.pData,
                pAttArray[TEST_CC6_ATT_CA10].Payload.pSubAttArray[TEST_CA10_ATT_SA1].PayloadSize);
        }
        // Retrieve data of sub-attribute CA11
        // Initialize optional attribute flags bitfield
        pCmdPayload->cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield = 0;
        // Retrieve data of sub-attribute SA1
        if (pAttArray[TEST_CC6_ATT_CA10]
                .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
                .Payload.pSubAttArray[TEST_CA11_ATT_SA1]
                .Payload.pData != NULL) {
            // Note presence of optional attribute SA1
            pCmdPayload->cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield |= TEST_CA11_ATT_SA1_FLAG;
            // Retrieve data of sub-attribute SA1
            memcpy(&(pCmdPayload->cc6_payload.ca10_payload.ca11_payload.sa1),
                pAttArray[TEST_CC6_ATT_CA10]
                    .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
                    .Payload.pSubAttArray[TEST_CA11_ATT_SA1]
                    .Payload.pData,
                pAttArray[TEST_CC6_ATT_CA10]
                    .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
                    .Payload.pSubAttArray[TEST_CA11_ATT_SA1]
                    .PayloadSize);
        }
        // Retrieve data of sub-attribute CA12
        // Retrieve data of sub-attribute SA4
        pCmdPayload->cc6_payload.ca10_payload.ca11_payload.ca12_payload.p_sa4 = pAttArray[TEST_CC6_ATT_CA10]
                                                                                    .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
                                                                                    .Payload.pSubAttArray[TEST_CA11_ATT_CA12]
                                                                                    .Payload.pSubAttArray[TEST_CA12_ATT_SA4]
                                                                                    .Payload.pData;
    }
}

/**
 * \fn static void LCSF_Bridge_TestGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload)
 * \brief Retrieve command data from its attribute array and store it in a payload
 *
 * \param cmdName name of the command
 * \param pAttArray pointer to the command attribute array
 * \param pPayload pointer to the payload to contain the command data
 * \return void
 */
static void LCSF_Bridge_TestGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, test_cmd_payload_t *pCmdPayload) {
    if (pAttArray == NULL) {
        return;
    }
    switch (cmdName) {
        case TEST_CMD_CC2:
            LCSF_Bridge_TestCC2GetData(pAttArray, pCmdPayload);
            break;

        case TEST_CMD_CC3:
            LCSF_Bridge_TestCC3GetData(pAttArray, pCmdPayload);
            break;

        case TEST_CMD_CC5:
            LCSF_Bridge_TestCC5GetData(pAttArray, pCmdPayload);
            break;

        case TEST_CMD_CC6:
            LCSF_Bridge_TestCC6GetData(pAttArray, pCmdPayload);
            break;

        default: // Commands that don't have payload
            return;
    }
}

/**
 * \fn static bool LCSF_Bridge_TestXFillAtt(lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload)
 * \brief Allocate and fill attribute array of command X from its payload
 *
 * \param pAttArrayAddr pointer to contain the attribute array
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */
static bool LCSF_Bridge_TestCC1FillAtt(lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Allocate attribute array
    if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_CMD_CC1_ATT_NB, (void *)pAttArrayAddr)) {
        return false;
    }
    // Intermediary variable
    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;
    // Fill data of attribute SA1
    pAttArray[TEST_CC1_ATT_SA1].PayloadSize = GetVLESize(pCmdPayload->cc1_payload.sa1);
    pAttArray[TEST_CC1_ATT_SA1].Payload.pData = &(pCmdPayload->cc1_payload.sa1);
    // Fill data of attribute SA2
    pAttArray[TEST_CC1_ATT_SA2].PayloadSize = GetVLESize(pCmdPayload->cc1_payload.sa2);
    pAttArray[TEST_CC1_ATT_SA2].Payload.pData = &(pCmdPayload->cc1_payload.sa2);
    // Fill data of attribute SA3
    pAttArray[TEST_CC1_ATT_SA3].PayloadSize = GetVLESize(pCmdPayload->cc1_payload.sa3);
    pAttArray[TEST_CC1_ATT_SA3].Payload.pData = &(pCmdPayload->cc1_payload.sa3);
    // Fill data of attribute SA4
    pAttArray[TEST_CC1_ATT_SA4].PayloadSize = pCmdPayload->cc1_payload.sa4Size;
    pAttArray[TEST_CC1_ATT_SA4].Payload.pData = pCmdPayload->cc1_payload.p_sa4;
    // Fill data of attribute SA5
    pAttArray[TEST_CC1_ATT_SA5].Payload.pData = pCmdPayload->cc1_payload.p_sa5;
    // Fill data of attribute SA6
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA6_FLAG) != 0) {
        pAttArray[TEST_CC1_ATT_SA6].PayloadSize = GetVLESize(pCmdPayload->cc1_payload.sa6);
        pAttArray[TEST_CC1_ATT_SA6].Payload.pData = &(pCmdPayload->cc1_payload.sa6);
    } else {
        pAttArray[TEST_CC1_ATT_SA6].Payload.pData = NULL;
    }
    // Fill data of attribute SA7
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA7_FLAG) != 0) {
        pAttArray[TEST_CC1_ATT_SA7].PayloadSize = GetVLESize(pCmdPayload->cc1_payload.sa7);
        pAttArray[TEST_CC1_ATT_SA7].Payload.pData = &(pCmdPayload->cc1_payload.sa7);
    } else {
        pAttArray[TEST_CC1_ATT_SA7].Payload.pData = NULL;
    }
    // Fill data of attribute SA8
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA8_FLAG) != 0) {
        pAttArray[TEST_CC1_ATT_SA8].PayloadSize = GetVLESize(pCmdPayload->cc1_payload.sa8);
        pAttArray[TEST_CC1_ATT_SA8].Payload.pData = &(pCmdPayload->cc1_payload.sa8);
    } else {
        pAttArray[TEST_CC1_ATT_SA8].Payload.pData = NULL;
    }
    // Fill data of attribute SA9
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA9_FLAG) != 0) {
        pAttArray[TEST_CC1_ATT_SA9].PayloadSize = pCmdPayload->cc1_payload.sa9Size;
        pAttArray[TEST_CC1_ATT_SA9].Payload.pData = pCmdPayload->cc1_payload.p_sa9;
    } else {
        pAttArray[TEST_CC1_ATT_SA9].Payload.pData = NULL;
    }
    // Fill data of attribute SA10
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA10_FLAG) != 0) {
        pAttArray[TEST_CC1_ATT_SA10].Payload.pData = pCmdPayload->cc1_payload.p_sa10;
    } else {
        pAttArray[TEST_CC1_ATT_SA10].Payload.pData = NULL;
    }
    // Fill data of attribute SA11
    pAttArray[TEST_CC1_ATT_SA11].PayloadSize = GetVLESize(pCmdPayload->cc1_payload.sa11);
    pAttArray[TEST_CC1_ATT_SA11].Payload.pData = &(pCmdPayload->cc1_payload.sa11);
    // Fill data of attribute SA12
    pAttArray[TEST_CC1_ATT_SA12].PayloadSize = sizeof(float);
    pAttArray[TEST_CC1_ATT_SA12].Payload.pData = &(pCmdPayload->cc1_payload.sa12);
    // Fill data of attribute SA13
    pAttArray[TEST_CC1_ATT_SA13].PayloadSize = sizeof(double);
    pAttArray[TEST_CC1_ATT_SA13].Payload.pData = &(pCmdPayload->cc1_payload.sa13);
    return true;
}

static bool LCSF_Bridge_TestCC3FillAtt(lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Allocate attribute array
    if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_CMD_CC3_ATT_NB, (void *)pAttArrayAddr)) {
        return false;
    }
    // Intermediary variable
    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;
    // Fill data of attribute SA1
    pAttArray[TEST_CC3_ATT_SA1].PayloadSize = GetVLESize(pCmdPayload->cc3_payload.sa1);
    pAttArray[TEST_CC3_ATT_SA1].Payload.pData = &(pCmdPayload->cc3_payload.sa1);
    // Fill data of attribute SA2
    pAttArray[TEST_CC3_ATT_SA2].PayloadSize = GetVLESize(pCmdPayload->cc3_payload.sa2);
    pAttArray[TEST_CC3_ATT_SA2].Payload.pData = &(pCmdPayload->cc3_payload.sa2);
    // Fill data of attribute SA3
    pAttArray[TEST_CC3_ATT_SA3].PayloadSize = GetVLESize(pCmdPayload->cc3_payload.sa3);
    pAttArray[TEST_CC3_ATT_SA3].Payload.pData = &(pCmdPayload->cc3_payload.sa3);
    // Fill data of attribute SA4
    pAttArray[TEST_CC3_ATT_SA4].PayloadSize = pCmdPayload->cc3_payload.sa4Size;
    pAttArray[TEST_CC3_ATT_SA4].Payload.pData = pCmdPayload->cc3_payload.p_sa4;
    // Fill data of attribute SA5
    pAttArray[TEST_CC3_ATT_SA5].Payload.pData = pCmdPayload->cc3_payload.p_sa5;
    // Fill data of attribute SA6
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA6_FLAG) != 0) {
        pAttArray[TEST_CC3_ATT_SA6].PayloadSize = GetVLESize(pCmdPayload->cc3_payload.sa6);
        pAttArray[TEST_CC3_ATT_SA6].Payload.pData = &(pCmdPayload->cc3_payload.sa6);
    } else {
        pAttArray[TEST_CC3_ATT_SA6].Payload.pData = NULL;
    }
    // Fill data of attribute SA7
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA7_FLAG) != 0) {
        pAttArray[TEST_CC3_ATT_SA7].PayloadSize = GetVLESize(pCmdPayload->cc3_payload.sa7);
        pAttArray[TEST_CC3_ATT_SA7].Payload.pData = &(pCmdPayload->cc3_payload.sa7);
    } else {
        pAttArray[TEST_CC3_ATT_SA7].Payload.pData = NULL;
    }
    // Fill data of attribute SA8
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA8_FLAG) != 0) {
        pAttArray[TEST_CC3_ATT_SA8].PayloadSize = GetVLESize(pCmdPayload->cc3_payload.sa8);
        pAttArray[TEST_CC3_ATT_SA8].Payload.pData = &(pCmdPayload->cc3_payload.sa8);
    } else {
        pAttArray[TEST_CC3_ATT_SA8].Payload.pData = NULL;
    }
    // Fill data of attribute SA9
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA9_FLAG) != 0) {
        pAttArray[TEST_CC3_ATT_SA9].PayloadSize = pCmdPayload->cc3_payload.sa9Size;
        pAttArray[TEST_CC3_ATT_SA9].Payload.pData = pCmdPayload->cc3_payload.p_sa9;
    } else {
        pAttArray[TEST_CC3_ATT_SA9].Payload.pData = NULL;
    }
    // Fill data of attribute SA10
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA10_FLAG) != 0) {
        pAttArray[TEST_CC3_ATT_SA10].Payload.pData = pCmdPayload->cc3_payload.p_sa10;
    } else {
        pAttArray[TEST_CC3_ATT_SA10].Payload.pData = NULL;
    }
    // Fill data of attribute SA11
    pAttArray[TEST_CC3_ATT_SA11].PayloadSize = GetVLESize(pCmdPayload->cc3_payload.sa11);
    pAttArray[TEST_CC3_ATT_SA11].Payload.pData = &(pCmdPayload->cc3_payload.sa11);
    // Fill data of attribute SA12
    pAttArray[TEST_CC3_ATT_SA12].PayloadSize = sizeof(float);
    pAttArray[TEST_CC3_ATT_SA12].Payload.pData = &(pCmdPayload->cc3_payload.sa12);
    // Fill data of attribute SA13
    pAttArray[TEST_CC3_ATT_SA13].PayloadSize = sizeof(double);
    pAttArray[TEST_CC3_ATT_SA13].Payload.pData = &(pCmdPayload->cc3_payload.sa13);
    return true;
}

static bool LCSF_Bridge_TestCC4FillAtt(lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Allocate attribute array
    if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_CMD_CC4_ATT_NB, (void *)pAttArrayAddr)) {
        return false;
    }
    // Intermediary variable
    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;
    lcsf_valid_att_t **pSubAttArray = NULL;
    // Fill data of attribute SA1
    pAttArray[TEST_CC4_ATT_SA1].PayloadSize = GetVLESize(pCmdPayload->cc4_payload.sa1);
    pAttArray[TEST_CC4_ATT_SA1].Payload.pData = &(pCmdPayload->cc4_payload.sa1);
    // Fill data of attribute CA1
    // Intermediary variable
    pSubAttArray = &(pAttArray[TEST_CC4_ATT_CA1].Payload.pSubAttArray);
    // Allocate sub-attribute array
    if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_ATT_CA1_SUBATT_NB, (void *)pSubAttArray)) {
        return false;
    }
    // Fill data of sub-attribute SA1
    pAttArray[TEST_CC4_ATT_CA1].Payload.pSubAttArray[TEST_CA1_ATT_SA1].PayloadSize =
        GetVLESize(pCmdPayload->cc4_payload.ca1_payload.sa1);
    pAttArray[TEST_CC4_ATT_CA1].Payload.pSubAttArray[TEST_CA1_ATT_SA1].Payload.pData =
        &(pCmdPayload->cc4_payload.ca1_payload.sa1);
    // Fill data of sub-attribute SA2
    pAttArray[TEST_CC4_ATT_CA1].Payload.pSubAttArray[TEST_CA1_ATT_SA2].PayloadSize =
        GetVLESize(pCmdPayload->cc4_payload.ca1_payload.sa2);
    pAttArray[TEST_CC4_ATT_CA1].Payload.pSubAttArray[TEST_CA1_ATT_SA2].Payload.pData =
        &(pCmdPayload->cc4_payload.ca1_payload.sa2);
    // Fill data of sub-attribute SA3
    if ((pCmdPayload->cc4_payload.ca1_payload.optAttFlagsBitfield & TEST_CA1_ATT_SA3_FLAG) != 0) {
        pAttArray[TEST_CC4_ATT_CA1].Payload.pSubAttArray[TEST_CA1_ATT_SA3].PayloadSize =
            GetVLESize(pCmdPayload->cc4_payload.ca1_payload.sa3);
        pAttArray[TEST_CC4_ATT_CA1].Payload.pSubAttArray[TEST_CA1_ATT_SA3].Payload.pData =
            &(pCmdPayload->cc4_payload.ca1_payload.sa3);
    } else {
        pAttArray[TEST_CC4_ATT_CA1].Payload.pSubAttArray[TEST_CA1_ATT_SA3].Payload.pData = NULL;
    }
    // Fill data of attribute CA2
    if ((pCmdPayload->cc4_payload.optAttFlagsBitfield & TEST_CC4_ATT_CA2_FLAG) != 0) {
        // Intermediary variable
        pSubAttArray = &(pAttArray[TEST_CC4_ATT_CA2].Payload.pSubAttArray);
        // Allocate sub-attribute array
        if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_ATT_CA2_SUBATT_NB, (void *)pSubAttArray)) {
            return false;
        }
        // Fill data of sub-attribute SA1
        if ((pCmdPayload->cc4_payload.ca2_payload.optAttFlagsBitfield & TEST_CA2_ATT_SA1_FLAG) != 0) {
            pAttArray[TEST_CC4_ATT_CA2].Payload.pSubAttArray[TEST_CA2_ATT_SA1].PayloadSize =
                GetVLESize(pCmdPayload->cc4_payload.ca2_payload.sa1);
            pAttArray[TEST_CC4_ATT_CA2].Payload.pSubAttArray[TEST_CA2_ATT_SA1].Payload.pData =
                &(pCmdPayload->cc4_payload.ca2_payload.sa1);
        } else {
            pAttArray[TEST_CC4_ATT_CA2].Payload.pSubAttArray[TEST_CA2_ATT_SA1].Payload.pData = NULL;
        }
        // Fill data of sub-attribute CA3
        // Intermediary variable
        pSubAttArray = &(pAttArray[TEST_CC4_ATT_CA2].Payload.pSubAttArray[TEST_CA2_ATT_CA3].Payload.pSubAttArray);
        // Allocate sub-attribute array
        if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_ATT_CA3_SUBATT_NB, (void *)pSubAttArray)) {
            return false;
        }
        // Fill data of sub-attribute SA1
        if ((pCmdPayload->cc4_payload.ca2_payload.ca3_payload.optAttFlagsBitfield & TEST_CA3_ATT_SA1_FLAG) != 0) {
            pAttArray[TEST_CC4_ATT_CA2]
                .Payload.pSubAttArray[TEST_CA2_ATT_CA3]
                .Payload.pSubAttArray[TEST_CA3_ATT_SA1]
                .PayloadSize = GetVLESize(pCmdPayload->cc4_payload.ca2_payload.ca3_payload.sa1);
            pAttArray[TEST_CC4_ATT_CA2]
                .Payload.pSubAttArray[TEST_CA2_ATT_CA3]
                .Payload.pSubAttArray[TEST_CA3_ATT_SA1]
                .Payload.pData = &(pCmdPayload->cc4_payload.ca2_payload.ca3_payload.sa1);
        } else {
            pAttArray[TEST_CC4_ATT_CA2]
                .Payload.pSubAttArray[TEST_CA2_ATT_CA3]
                .Payload.pSubAttArray[TEST_CA3_ATT_SA1]
                .Payload.pData = NULL;
        }
        // Fill data of sub-attribute CA4
        // Intermediary variable
        pSubAttArray = &(pAttArray[TEST_CC4_ATT_CA2]
                             .Payload.pSubAttArray[TEST_CA2_ATT_CA3]
                             .Payload.pSubAttArray[TEST_CA3_ATT_CA4]
                             .Payload.pSubAttArray);
        // Allocate sub-attribute array
        if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_ATT_CA4_SUBATT_NB, (void *)pSubAttArray)) {
            return false;
        }
        // Fill data of sub-attribute SA4
        pAttArray[TEST_CC4_ATT_CA2]
            .Payload.pSubAttArray[TEST_CA2_ATT_CA3]
            .Payload.pSubAttArray[TEST_CA3_ATT_CA4]
            .Payload.pSubAttArray[TEST_CA4_ATT_SA4]
            .PayloadSize = pCmdPayload->cc4_payload.ca2_payload.ca3_payload.ca4_payload.sa4Size;
        pAttArray[TEST_CC4_ATT_CA2]
            .Payload.pSubAttArray[TEST_CA2_ATT_CA3]
            .Payload.pSubAttArray[TEST_CA3_ATT_CA4]
            .Payload.pSubAttArray[TEST_CA4_ATT_SA4]
            .Payload.pData = pCmdPayload->cc4_payload.ca2_payload.ca3_payload.ca4_payload.p_sa4;
    } else {
        pAttArray[TEST_CC4_ATT_CA2].Payload.pData = NULL;
    }
    return true;
}

static bool LCSF_Bridge_TestCC6FillAtt(lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Allocate attribute array
    if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_CMD_CC6_ATT_NB, (void *)pAttArrayAddr)) {
        return false;
    }
    // Intermediary variable
    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;
    lcsf_valid_att_t **pSubAttArray = NULL;
    // Fill data of attribute SA4
    pAttArray[TEST_CC6_ATT_SA4].PayloadSize = pCmdPayload->cc6_payload.sa4Size;
    pAttArray[TEST_CC6_ATT_SA4].Payload.pData = pCmdPayload->cc6_payload.p_sa4;
    // Fill data of attribute CA9
    // Intermediary variable
    pSubAttArray = &(pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray);
    // Allocate sub-attribute array
    if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_ATT_CA9_SUBATT_NB, (void *)pSubAttArray)) {
        return false;
    }
    // Fill data of sub-attribute SA1
    pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA1].PayloadSize =
        GetVLESize(pCmdPayload->cc6_payload.ca9_payload.sa1);
    pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA1].Payload.pData =
        &(pCmdPayload->cc6_payload.ca9_payload.sa1);
    // Fill data of sub-attribute SA2
    pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA2].PayloadSize =
        GetVLESize(pCmdPayload->cc6_payload.ca9_payload.sa2);
    pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA2].Payload.pData =
        &(pCmdPayload->cc6_payload.ca9_payload.sa2);
    // Fill data of sub-attribute SA3
    if ((pCmdPayload->cc6_payload.ca9_payload.optAttFlagsBitfield & TEST_CA9_ATT_SA3_FLAG) != 0) {
        pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA3].PayloadSize =
            GetVLESize(pCmdPayload->cc6_payload.ca9_payload.sa3);
        pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA3].Payload.pData =
            &(pCmdPayload->cc6_payload.ca9_payload.sa3);
    } else {
        pAttArray[TEST_CC6_ATT_CA9].Payload.pSubAttArray[TEST_CA9_ATT_SA3].Payload.pData = NULL;
    }
    // Fill data of attribute CA10
    if ((pCmdPayload->cc6_payload.optAttFlagsBitfield & TEST_CC6_ATT_CA10_FLAG) != 0) {
        // Intermediary variable
        pSubAttArray = &(pAttArray[TEST_CC6_ATT_CA10].Payload.pSubAttArray);
        // Allocate sub-attribute array
        if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_ATT_CA10_SUBATT_NB, (void *)pSubAttArray)) {
            return false;
        }
        // Fill data of sub-attribute SA1
        if ((pCmdPayload->cc6_payload.ca10_payload.optAttFlagsBitfield & TEST_CA10_ATT_SA1_FLAG) != 0) {
            pAttArray[TEST_CC6_ATT_CA10].Payload.pSubAttArray[TEST_CA10_ATT_SA1].PayloadSize =
                GetVLESize(pCmdPayload->cc6_payload.ca10_payload.sa1);
            pAttArray[TEST_CC6_ATT_CA10].Payload.pSubAttArray[TEST_CA10_ATT_SA1].Payload.pData =
                &(pCmdPayload->cc6_payload.ca10_payload.sa1);
        } else {
            pAttArray[TEST_CC6_ATT_CA10].Payload.pSubAttArray[TEST_CA10_ATT_SA1].Payload.pData = NULL;
        }
        // Fill data of sub-attribute CA11
        // Intermediary variable
        pSubAttArray = &(pAttArray[TEST_CC6_ATT_CA10].Payload.pSubAttArray[TEST_CA10_ATT_CA11].Payload.pSubAttArray);
        // Allocate sub-attribute array
        if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_ATT_CA11_SUBATT_NB, (void *)pSubAttArray)) {
            return false;
        }
        // Fill data of sub-attribute SA1
        if ((pCmdPayload->cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield & TEST_CA11_ATT_SA1_FLAG) != 0) {
            pAttArray[TEST_CC6_ATT_CA10]
                .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
                .Payload.pSubAttArray[TEST_CA11_ATT_SA1]
                .PayloadSize = GetVLESize(pCmdPayload->cc6_payload.ca10_payload.ca11_payload.sa1);
            pAttArray[TEST_CC6_ATT_CA10]
                .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
                .Payload.pSubAttArray[TEST_CA11_ATT_SA1]
                .Payload.pData = &(pCmdPayload->cc6_payload.ca10_payload.ca11_payload.sa1);
        } else {
            pAttArray[TEST_CC6_ATT_CA10]
                .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
                .Payload.pSubAttArray[TEST_CA11_ATT_SA1]
                .Payload.pData = NULL;
        }
        // Fill data of sub-attribute CA12
        // Intermediary variable
        pSubAttArray = &(pAttArray[TEST_CC6_ATT_CA10]
                             .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
                             .Payload.pSubAttArray[TEST_CA11_ATT_CA12]
                             .Payload.pSubAttArray);
        // Allocate sub-attribute array
        if (!FiloGet(&LcsfBridgeTestInfo.Filo, LCSF_TEST_ATT_CA12_SUBATT_NB, (void *)pSubAttArray)) {
            return false;
        }
        // Fill data of sub-attribute SA4
        pAttArray[TEST_CC6_ATT_CA10]
            .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
            .Payload.pSubAttArray[TEST_CA11_ATT_CA12]
            .Payload.pSubAttArray[TEST_CA12_ATT_SA4]
            .PayloadSize = pCmdPayload->cc6_payload.ca10_payload.ca11_payload.ca12_payload.sa4Size;
        pAttArray[TEST_CC6_ATT_CA10]
            .Payload.pSubAttArray[TEST_CA10_ATT_CA11]
            .Payload.pSubAttArray[TEST_CA11_ATT_CA12]
            .Payload.pSubAttArray[TEST_CA12_ATT_SA4]
            .Payload.pData = pCmdPayload->cc6_payload.ca10_payload.ca11_payload.ca12_payload.p_sa4;
    } else {
        pAttArray[TEST_CC6_ATT_CA10].Payload.pData = NULL;
    }
    return true;
}

/**
 * \fn static bool LCSF_Bridge_TestFillCmdAtt(uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload)
 * \brief Fill the attribute array of a command from its payload
 *
 * \param cmdName name of the command
 * \param pAttArrayAddr pointer to contain the attribute array
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */
static bool LCSF_Bridge_TestFillCmdAtt(
    uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, test_cmd_payload_t *pCmdPayload) {
    switch (cmdName) {
        case TEST_CMD_CC1:
            return LCSF_Bridge_TestCC1FillAtt(pAttArrayAddr, pCmdPayload);

        case TEST_CMD_CC3:
            return LCSF_Bridge_TestCC3FillAtt(pAttArrayAddr, pCmdPayload);

        case TEST_CMD_CC4:
            return LCSF_Bridge_TestCC4FillAtt(pAttArrayAddr, pCmdPayload);

        case TEST_CMD_CC6:
            return LCSF_Bridge_TestCC6FillAtt(pAttArrayAddr, pCmdPayload);

        default: // Commands that don't have attributes
            *pAttArrayAddr = NULL;
            return true;
    }
}

// *** Public Functions ***

bool LCSF_Bridge_TestInit(void) {
    return FiloInit(
        &LcsfBridgeTestInfo.Filo, LcsfBridgeTestInfo.FiloData, LCSF_BRIDGE_TEST_FILO_SIZE, sizeof(lcsf_valid_att_t));
}

bool LCSF_Bridge_TestReceive(lcsf_valid_cmd_t *pValidCmd) {
    uint16_t cmdName = LCSF_Bridge_Test_CMDID2CMDNAME(pValidCmd->CmdId);
    test_cmd_payload_t *pCmdPayload = &LcsfBridgeTestInfo.CmdPayload;

    LCSF_Bridge_TestGetCmdData(cmdName, pValidCmd->pAttArray, pCmdPayload);
    return Test_MainExecute(cmdName, pCmdPayload);
}

int LCSF_Bridge_TestEncode(uint_fast16_t cmdName, test_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize) {
    lcsf_valid_cmd_t sendCmd;
    sendCmd.CmdId = LCSF_Bridge_Test_CMDNAME2CMDID[cmdName];
    FiloFreeAll(&LcsfBridgeTestInfo.Filo);

    if (!LCSF_Bridge_TestFillCmdAtt(cmdName, &(sendCmd.pAttArray), pCmdPayload)) {
        return -1;
    }
    return LCSF_ValidatorEncode(LCSF_TEST_PROTOCOL_ID, &sendCmd, pBuffer, buffSize);
}
