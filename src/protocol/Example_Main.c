/**
 * \file Example_Main.c
 * \brief Example protocol module
 * \author Jean-Roland Gosse

    This file is part of LCSF C Stack.

    LCSF C Stack is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LCSF C Stack is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this project. If not, see <https://www.gnu.org/licenses/>
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include <LCSF_config.h>
#include <LCSF_Bridge_Example.h>
#include <Example_Main.h>

// *** Definitions ***
// --- Private Types ---

enum _example_error_code_enum {
    EXAMPLE_ERROR_INVALID_JMP_ADDR,
    EXAMPLE_ERROR_INVALID_COLOR,
    EXAMPLE_ERROR_WRONG_CMD,
};

typedef struct _example_info {
    const example_init_desc_t *pInitDesc;
    example_cmd_payload_t *pSendCmdPayload;
} example_info_t;

// --- Private Constants ---
// --- Private Function Prototypes ---
static bool Example_MainCheckAddrValidity(uintptr_t addr);
static bool Example_MainSendError(uint8_t errorCode);
static bool Example_MainSendAck(void);
static bool Example_MainSendCommand(uint16_t cmdName, bool hasPayload);
static bool Example_MainExecutePING(void);
static bool Example_MainExecuteRESET(void);
static bool Example_MainExecuteABORT(void);
static bool Example_MainExecuteJUMP(example_cmd_payload_t *pCmdPayload);
static bool Example_MainExecuteCOLOR_SPACE(example_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static example_info_t ExampleInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static bool Example_MainCheckAddrValidity(uintptr_t addr)
 * \brief Check validity of an address
 *
 * \param addr address to check
 * \return bool: true if address is valid
 */
static bool Example_MainCheckAddrValidity(uintptr_t addr) {

    if (((void *)addr != NULL) && (addr >= 0x20400000) && (addr <= 0x20410000)) {
        return true;
    } else {
        return false;
    }
}

/**
 * \fn static bool Example_MainSendError(uint8_t errorCode)
 * \brief Send an example error command
 *
 * \param errorCode error code to send
 * \return bool: true if operation was a success
 */
static bool Example_MainSendError(uint8_t errorCode) {
    ExampleInfo.pSendCmdPayload->error_payload.error_code = errorCode;
    return Example_MainSendCommand(EXAMPLE_CMD_ERROR, true);
}

/**
 * \fn static bool Example_MainSendAck(void)
 * \brief Send an example acknowledge command
 *
 * \return bool: true if operation was a success
 */
static bool Example_MainSendAck(void) {
    return Example_MainSendCommand(EXAMPLE_CMD_ACK, false);
}

/**
 * \fn static bool Example_MainSendCommand(uint16_t cmdName, bool hasPayload)
 * \brief Send an example command
 *
 * \param cmdName name of the command to send
 * \param hasPayload indicates if command has a payload or not
 * \return bool: true if operation was a success
 */
static bool Example_MainSendCommand(uint16_t cmdName, bool hasPayload) {

    if (cmdName >= EXAMPLE_CMD_COUNT) {
        return false;
    }
    if (hasPayload) {
        example_cmd_payload_t *pCmdPayload = ExampleInfo.pSendCmdPayload;
        return LCSF_Bridge_ExampleSend(cmdName, pCmdPayload);
    } else {
        return LCSF_Bridge_ExampleSend(cmdName, NULL);
    }
}

/**
 * \fn static bool Example_MainExecuteX(void)
 * \brief Execute command X (no payload)
 *
 * \return bool: true if operation was a success
 */

/**
 * \fn static bool Example_MainExecuteX(example_cmd_payload_t *pCmdPayload)
 * \brief Execute command X (with payload)
 *
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */

static bool Example_MainExecutePING(void) {
    return Example_MainSendCommand(EXAMPLE_CMD_PING, false);
}

static bool Example_MainExecuteRESET(void) {
    if (!Example_MainSendAck()) {
        return false;
    }
    // System reset
    return true;
}

static bool Example_MainExecuteABORT(void) {
    // Abort stuff
    return Example_MainSendAck();
}

static bool Example_MainExecuteJUMP(example_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    uint32_t m_jump_addr = pCmdPayload->jump_payload.jump_addr;

    if (!Example_MainCheckAddrValidity((uintptr_t)m_jump_addr)) {
        return Example_MainSendError(EXAMPLE_ERROR_INVALID_JMP_ADDR);
    }
    if (!Example_MainSendAck()) {
        return false;
    }
    // System goto jump addr
    return true;
}

static bool Example_MainExecuteCOLOR_SPACE(example_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Initialize payload
    memset(ExampleInfo.pSendCmdPayload, 0, sizeof(example_cmd_payload_t));

    if ((pCmdPayload->color_space_payload.optAttFlagsBitfield & EXAMPLE_COLOR_SPACE_ATT_YUV_FLAG) != 0) {
        uint16_t m_y = pCmdPayload->color_space_payload.yuv_payload.y;
        uint16_t m_u = pCmdPayload->color_space_payload.yuv_payload.u;
        uint16_t m_v = pCmdPayload->color_space_payload.yuv_payload.v;

        if ((m_y + m_u + m_v) <= 1023) {
            ExampleInfo.pSendCmdPayload->color_space_payload.optAttFlagsBitfield |= EXAMPLE_COLOR_SPACE_ATT_YUV_FLAG;
            ExampleInfo.pSendCmdPayload->color_space_payload.yuv_payload.y = m_y;
            ExampleInfo.pSendCmdPayload->color_space_payload.yuv_payload.u = m_u;
            ExampleInfo.pSendCmdPayload->color_space_payload.yuv_payload.v = m_v;
            return Example_MainSendCommand(EXAMPLE_CMD_COLOR_SPACE, true);
        } else {
            return Example_MainSendError(EXAMPLE_ERROR_INVALID_COLOR);
        }
    }
    if ((pCmdPayload->color_space_payload.optAttFlagsBitfield & EXAMPLE_COLOR_SPACE_ATT_RGB_FLAG) != 0) {
        uint16_t m_r = pCmdPayload->color_space_payload.rgb_payload.r;
        uint16_t m_g = pCmdPayload->color_space_payload.rgb_payload.g;
        uint16_t m_b = pCmdPayload->color_space_payload.rgb_payload.b;

        if ((m_r + m_g + m_b) <= 1023) {
            ExampleInfo.pSendCmdPayload->color_space_payload.optAttFlagsBitfield |= EXAMPLE_COLOR_SPACE_ATT_RGB_FLAG;
            ExampleInfo.pSendCmdPayload->color_space_payload.rgb_payload.r = m_r;
            ExampleInfo.pSendCmdPayload->color_space_payload.rgb_payload.g = m_g;
            ExampleInfo.pSendCmdPayload->color_space_payload.rgb_payload.b = m_b;
            return Example_MainSendCommand(EXAMPLE_CMD_COLOR_SPACE, true);
        } else {
            return Example_MainSendError(EXAMPLE_ERROR_INVALID_COLOR);
        }
    }
    return Example_MainSendAck();
}

// *** Public Functions ***

bool Example_MainInit(const example_init_desc_t * pInitDesc) {
    ExampleInfo.pInitDesc = pInitDesc;
    ExampleInfo.pSendCmdPayload = (example_cmd_payload_t *)MEM_ALLOC(sizeof(example_cmd_payload_t));
    return true;
}

bool Example_MainCommandExecute(uint16_t cmdName, example_cmd_payload_t *pCmdPayload) {

    switch (cmdName) {
        case EXAMPLE_CMD_PING:
            return Example_MainExecutePING();
        break;

        case EXAMPLE_CMD_RESET:
            return Example_MainExecuteRESET();
        break;

        case EXAMPLE_CMD_ABORT:
            return Example_MainExecuteABORT();
        break;

        case EXAMPLE_CMD_JUMP:
            return Example_MainExecuteJUMP(pCmdPayload);
        break;

        case EXAMPLE_CMD_COLOR_SPACE:
            return Example_MainExecuteCOLOR_SPACE(pCmdPayload);
        break;

        default:
            return Example_MainSendError(EXAMPLE_ERROR_WRONG_CMD);
        break;
    }
}