/**
 * \file LCSF_Bridge_Example.c
 * \brief Example of a LCSF protocol bridge
 * \author Jean-Roland Gosse
 *
 * This file is part of LCSF C Stack.
 *
 * LCSF C Stack is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCSF C Stack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this project. If not, see <https://www.gnu.org/licenses/>
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include "LCSF_Bridge_Example.h"
#include <Filo.h>
#include <LCSF_Config.h>
#include <LCSF_Transcoder.h>
#include <LCSF_Validator.h>

// *** Definitions ***
// --- Private Types ---

// Module information structure
typedef struct _lcsf_bridge_example_info {
    uint8_t FiloData[LCSF_EXAMPLE_BRIDGE_FILO_SIZE * sizeof(lcsf_valid_att_t)];
    filo_desc_t Filo;
    example_cmd_payload_t CmdPayload;
} lcsf_bridge_example_info_t;

// --- Private Constants ---
// Array to convert command name value to their lcsf command id
const uint16_t LCSF_Bridge_Example_CMDNAME2CMDID[LCSF_EXAMPLE_CMD_NB] = {
    LCSF_EXAMPLE_CMD_ID_PING,
    LCSF_EXAMPLE_CMD_ID_RESET,
    LCSF_EXAMPLE_CMD_ID_ERROR,
    LCSF_EXAMPLE_CMD_ID_ACK,
    LCSF_EXAMPLE_CMD_ID_ABORT,
    LCSF_EXAMPLE_CMD_ID_JUMP,
    LCSF_EXAMPLE_CMD_ID_COLOR_SPACE,
};

// --- Private Function Prototypes ---
static uint16_t LCSF_Bridge_Example_CMDID2CMDNAME(uint_fast16_t cmdId);
static void LCSF_Bridge_ExampleJUMPGetData(lcsf_valid_att_t *pAttArray, example_cmd_payload_t *pCmdPayload);
static void LCSF_Bridge_ExampleCOLOR_SPACEGetData(lcsf_valid_att_t *pAttArray, example_cmd_payload_t *pCmdPayload);
static void LCSF_Bridge_ExampleGetCmdData(
    uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, example_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_ExampleERRORFillAtt(lcsf_valid_att_t **pAttArray, example_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_ExampleCOLOR_SPACEFillAtt(lcsf_valid_att_t **pAttArray, example_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_ExampleFillCmdAtt(
    uint_fast16_t cmdName, lcsf_valid_att_t **pAttArray, example_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static lcsf_bridge_example_info_t LcsfBridgeExampleInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static uint16_t LCSF_Bridge_Example_CMDID2CMDNAME(uint_fast16_t cmdId)
 * \brief Translate an lcsf command id to its name value
 *
 * \param cmdId lcsf command identifier to translate
 * \return uint16_t: name value of the command
 */
static uint16_t LCSF_Bridge_Example_CMDID2CMDNAME(uint_fast16_t cmdId) {
    switch (cmdId) {
        default:
        case LCSF_EXAMPLE_CMD_ID_PING:
            return EXAMPLE_CMD_PING;
        case LCSF_EXAMPLE_CMD_ID_RESET:
            return EXAMPLE_CMD_RESET;
        case LCSF_EXAMPLE_CMD_ID_ERROR:
            return EXAMPLE_CMD_ERROR;
        case LCSF_EXAMPLE_CMD_ID_ACK:
            return EXAMPLE_CMD_ACK;
        case LCSF_EXAMPLE_CMD_ID_ABORT:
            return EXAMPLE_CMD_ABORT;
        case LCSF_EXAMPLE_CMD_ID_JUMP:
            return EXAMPLE_CMD_JUMP;
        case LCSF_EXAMPLE_CMD_ID_COLOR_SPACE:
            return EXAMPLE_CMD_COLOR_SPACE;
    }
}

/**
 * \fn static void LCSF_Bridge_ExampleXGetData(lcsf_valid_att_t *pAttArray, example_cmd_payload_t *pCmdPayload)
 * \brief Retrieve data of command X from its valid attribute array and store it in a payload
 *
 * \param pAttArray pointer to the command attribute array
 * \param pCmdPayload pointer to the payload to contain the command data
 * \return void
 */
static void LCSF_Bridge_ExampleJUMPGetData(lcsf_valid_att_t *pAttArray, example_cmd_payload_t *pCmdPayload) {
    // Retrieve data of attribute JUMP_ADDR
    memcpy(&(pCmdPayload->jump_payload.jump_addr), pAttArray[EXAMPLE_JUMP_ATT_JUMP_ADDR].Payload.pData, sizeof(uint32_t));
}

static void LCSF_Bridge_ExampleCOLOR_SPACEGetData(lcsf_valid_att_t *pAttArray, example_cmd_payload_t *pCmdPayload) {
    // Initialize optional attribute flags bitfield
    pCmdPayload->color_space_payload.optAttFlagsBitfield = 0;

    // Retrieve data of attribute YUV
    if (pAttArray[EXAMPLE_COLOR_SPACE_ATT_YUV].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->color_space_payload.optAttFlagsBitfield |= EXAMPLE_COLOR_SPACE_ATT_YUV_FLAG;
        // Retrieve data of sub-attribute Y
        memcpy(&(pCmdPayload->color_space_payload.yuv_payload.y),
            pAttArray[EXAMPLE_COLOR_SPACE_ATT_YUV].Payload.pSubAttArray[EXAMPLE_YUV_ATT_Y].Payload.pData, sizeof(uint16_t));
        // Retrieve data of sub-attribute U
        memcpy(&(pCmdPayload->color_space_payload.yuv_payload.u),
            pAttArray[EXAMPLE_COLOR_SPACE_ATT_YUV].Payload.pSubAttArray[EXAMPLE_YUV_ATT_U].Payload.pData, sizeof(uint16_t));
        // Retrieve data of sub-attribute V
        memcpy(&(pCmdPayload->color_space_payload.yuv_payload.v),
            pAttArray[EXAMPLE_COLOR_SPACE_ATT_YUV].Payload.pSubAttArray[EXAMPLE_YUV_ATT_V].Payload.pData, sizeof(uint16_t));
    }
    // Retrieve data of attribute RGB
    if (pAttArray[EXAMPLE_COLOR_SPACE_ATT_RGB].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->color_space_payload.optAttFlagsBitfield |= EXAMPLE_COLOR_SPACE_ATT_RGB_FLAG;
        // Retrieve data of sub-attribute R
        memcpy(&(pCmdPayload->color_space_payload.rgb_payload.r),
            pAttArray[EXAMPLE_COLOR_SPACE_ATT_RGB].Payload.pSubAttArray[EXAMPLE_RGB_ATT_R].Payload.pData, sizeof(uint16_t));
        // Retrieve data of sub-attribute G
        memcpy(&(pCmdPayload->color_space_payload.rgb_payload.g),
            pAttArray[EXAMPLE_COLOR_SPACE_ATT_RGB].Payload.pSubAttArray[EXAMPLE_RGB_ATT_G].Payload.pData, sizeof(uint16_t));
        // Retrieve data of sub-attribute B
        memcpy(&(pCmdPayload->color_space_payload.rgb_payload.b),
            pAttArray[EXAMPLE_COLOR_SPACE_ATT_RGB].Payload.pSubAttArray[EXAMPLE_RGB_ATT_B].Payload.pData, sizeof(uint16_t));
    }
}

/**
 * \fn static void LCSF_Bridge_ExampleGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, example_cmd_payload_t *pCmdPayload)
 * \brief Retrieve command data from its attribute array and store it in a payload
 *
 * \param cmdName name of the command
 * \param pAttArray pointer to the command attribute array
 * \param pPayload pointer to the payload to contain the command data
 * \return void
 */
static void LCSF_Bridge_ExampleGetCmdData(
    uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, example_cmd_payload_t *pCmdPayload) {
    if (pAttArray == NULL) {
        return;
    }
    switch (cmdName) {
        case EXAMPLE_CMD_JUMP:
            LCSF_Bridge_ExampleJUMPGetData(pAttArray, pCmdPayload);
            break;

        case EXAMPLE_CMD_COLOR_SPACE:
            LCSF_Bridge_ExampleCOLOR_SPACEGetData(pAttArray, pCmdPayload);
            break;

        default: // Commands that don't have payload
            return;
    }
}

/**
 * \fn static bool LCSF_Bridge_ExampleXFillAtt(lcsf_valid_att_t **pAttArray, example_cmd_payload_t *pCmdPayload)
 * \brief Allocate and fill attribute array of command X from its payload
 *
 * \param pAttArray pointer to contain the attribute array
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */
static bool LCSF_Bridge_ExampleERRORFillAtt(lcsf_valid_att_t **pAttArray, example_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    if (!FiloGet(&LcsfBridgeExampleInfo.Filo, LCSF_EXAMPLE_CMD_ERROR_ATT_NB, (void **)pAttArray)) {
        return false;
    }
    // Fill data of attribute ERROR_CODE
    ((*pAttArray)[EXAMPLE_ERROR_ATT_ERROR_CODE]).Payload.pData = &(pCmdPayload->error_payload.error_code);
    return true;
}

static bool LCSF_Bridge_ExampleCOLOR_SPACEFillAtt(lcsf_valid_att_t **pAttArray, example_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    if (!FiloGet(&LcsfBridgeExampleInfo.Filo, LCSF_EXAMPLE_CMD_COLOR_SPACE_ATT_NB, (void **)pAttArray)) {
        return false;
    }
    // Fill data of attribute YUV
    if ((pCmdPayload->color_space_payload.optAttFlagsBitfield & EXAMPLE_COLOR_SPACE_ATT_YUV_FLAG) != 0) {
        lcsf_valid_att_t **pSubAttArray = &(((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_YUV]).Payload.pSubAttArray);

        if (!FiloGet(&LcsfBridgeExampleInfo.Filo, LCSF_EXAMPLE_ATT_YUV_SUBATT_NB, (void **)pSubAttArray)) {
            return false;
        }
        // Fill data of sub-attribute Y
        ((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_YUV]).Payload.pSubAttArray[EXAMPLE_YUV_ATT_Y].Payload.pData =
            &(pCmdPayload->color_space_payload.yuv_payload.y);
        // Fill data of sub-attribute U
        ((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_YUV]).Payload.pSubAttArray[EXAMPLE_YUV_ATT_U].Payload.pData =
            &(pCmdPayload->color_space_payload.yuv_payload.u);
        // Fill data of sub-attribute V
        ((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_YUV]).Payload.pSubAttArray[EXAMPLE_YUV_ATT_V].Payload.pData =
            &(pCmdPayload->color_space_payload.yuv_payload.v);
    } else {
        ((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_YUV]).Payload.pData = NULL;
    }
    // Fill data of attribute RGB
    if ((pCmdPayload->color_space_payload.optAttFlagsBitfield & EXAMPLE_COLOR_SPACE_ATT_RGB_FLAG) != 0) {
        lcsf_valid_att_t **pSubAttArray = &(((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_YUV]).Payload.pSubAttArray);

        if (!FiloGet(&LcsfBridgeExampleInfo.Filo, LCSF_EXAMPLE_ATT_RGB_SUBATT_NB, (void **)pSubAttArray)) {
            return false;
        }
        // Fill data of sub-attribute R
        ((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_RGB]).Payload.pSubAttArray[EXAMPLE_RGB_ATT_R].Payload.pData =
            &(pCmdPayload->color_space_payload.rgb_payload.r);
        // Fill data of sub-attribute G
        ((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_RGB]).Payload.pSubAttArray[EXAMPLE_RGB_ATT_G].Payload.pData =
            &(pCmdPayload->color_space_payload.rgb_payload.g);
        // Fill data of sub-attribute B
        ((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_RGB]).Payload.pSubAttArray[EXAMPLE_RGB_ATT_B].Payload.pData =
            &(pCmdPayload->color_space_payload.rgb_payload.b);
    } else {
        ((*pAttArray)[EXAMPLE_COLOR_SPACE_ATT_RGB]).Payload.pData = NULL;
    }
    return true;
}

/**
 * \fnstatic bool LCSF_Bridge_ExampleFillCmdAtt(uint_fast16_t cmdName, lcsf_valid_att_t **pAttArray, example_cmd_payload_t *pCmdPayload)
 * \brief Fill the attribute array of a command from its payload
 *
 * \param cmdName name of the command
 * \param pAttArray pointer to contain the attribute array
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */
static bool LCSF_Bridge_ExampleFillCmdAtt(
    uint_fast16_t cmdName, lcsf_valid_att_t **pAttArray, example_cmd_payload_t *pCmdPayload) {
    switch (cmdName) {
        case EXAMPLE_CMD_ERROR:
            return LCSF_Bridge_ExampleERRORFillAtt(pAttArray, pCmdPayload);
            break;

        case EXAMPLE_CMD_COLOR_SPACE:
            return LCSF_Bridge_ExampleCOLOR_SPACEFillAtt(pAttArray, pCmdPayload);
            break;

        default: // Commands that don't have attributes
            *pAttArray = NULL;
            return true;
            break;
    }
}

// *** Public Functions ***

bool LCSF_Bridge_ExampleInit(void) {
    return FiloInit(&LcsfBridgeExampleInfo.Filo, LcsfBridgeExampleInfo.FiloData, LCSF_EXAMPLE_BRIDGE_FILO_SIZE,
        sizeof(lcsf_valid_att_t));
}

bool LCSF_Bridge_ExampleReceive(lcsf_valid_cmd_t *pValidCmd) {
    uint16_t cmdName = LCSF_Bridge_Example_CMDID2CMDNAME(pValidCmd->CmdId);
    example_cmd_payload_t *pCmdPayload = &LcsfBridgeExampleInfo.CmdPayload;

    LCSF_Bridge_ExampleGetCmdData(cmdName, pValidCmd->pAttArray, pCmdPayload);
    return Example_MainCommandExecute(cmdName, pCmdPayload);
}

int LCSF_Bridge_ExampleEncode(uint_fast16_t cmdName, example_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize) {
    lcsf_valid_cmd_t sendCmd;
    sendCmd.CmdId = LCSF_Bridge_Example_CMDNAME2CMDID[cmdName];
    FiloFreeAll(&LcsfBridgeExampleInfo.Filo);

    if (!LCSF_Bridge_ExampleFillCmdAtt(cmdName, &(sendCmd.pAttArray), pCmdPayload)) {
        return -1;
    }
    return LCSF_ValidatorEncode(LCSF_EXAMPLE_PROTOCOL_ID, &sendCmd, pBuffer, buffSize);
}