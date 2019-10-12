/**
 * \file Example_Main.h
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

#ifndef Example_main_h
#define Example_main_h

// *** Libraries include ***
// Standard lib
#include <stdbool.h>
#include <stdint.h>
// Custom lib

// *** Definitions ***
// --- Public Types ---

// Command name enum
enum _example_cmd_names {
    EXAMPLE_CMD_PING,
    EXAMPLE_CMD_RESET,
    EXAMPLE_CMD_ERROR,
    EXAMPLE_CMD_ACK,
    EXAMPLE_CMD_ABORT,
    EXAMPLE_CMD_JUMP,
    EXAMPLE_CMD_COLOR_SPACE,
    EXAMPLE_CMD_COUNT,
};

// Attributes enums
enum _example_color_space_att_names {
    EXAMPLE_COLOR_SPACE_ATT_YUV,
    EXAMPLE_COLOR_SPACE_ATT_RGB,
};

enum _example_error_att_names {
    EXAMPLE_ERROR_ATT_ERROR_CODE,
};

enum _example_jump_att_names {
    EXAMPLE_JUMP_ATT_JUMP_ADDR,
};

enum _example_rgb_att_names {
    EXAMPLE_RGB_ATT_R,
    EXAMPLE_RGB_ATT_G,
    EXAMPLE_RGB_ATT_B,
};

enum _example_yuv_att_names {
    EXAMPLE_YUV_ATT_Y,
    EXAMPLE_YUV_ATT_U,
    EXAMPLE_YUV_ATT_V,
};

// Attribute with sub-attributes structures
typedef struct _example_rgb_att_payload {
    uint16_t r;
    uint16_t g;
    uint16_t b;
} example_rgb_att_payload_t;

typedef struct _example_yuv_att_payload {
    uint16_t y;
    uint16_t u;
    uint16_t v;
} example_yuv_att_payload_t;

// Commands data structures
typedef struct _example_error_att_payload {
    uint8_t error_code;
} example_error_att_payload_t;

typedef struct _example_jump_att_payload {
    uint32_t jump_addr;
} example_jump_att_payload_t;

#define EXAMPLE_COLOR_SPACE_ATT_YUV_FLAG (1 << 0)
#define EXAMPLE_COLOR_SPACE_ATT_RGB_FLAG (1 << 1)
typedef struct _example_color_space_att_payload {
    uint8_t optAttFlagsBitfield;
    example_yuv_att_payload_t yuv_payload;
    example_rgb_att_payload_t rgb_payload;
} example_color_space_att_payload_t;

// Command payload union
typedef union _example_cmd_payload {
    example_error_att_payload_t error_payload;
    example_jump_att_payload_t jump_payload;
    example_color_space_att_payload_t color_space_payload;
} example_cmd_payload_t;

// Command send callback function prototype
typedef bool Example_SendCallback(uint8_t cmdName, example_cmd_payload_t *pCmdPayload);

// Module initialization descriptor
typedef struct _example_init_desc {
    uint8_t FooValue;
} example_init_desc_t;

// --- Public Function Prototypes ---

/**
 * \fn bool Example_MainInit(const example_init_desc_t *pInitDesc)
 * \brief Initialize the module
 *
 * \param pInitDesc pointer to module initialization descriptor
 * \return bool: true if operation was a success
 */
bool Example_MainInit(const example_init_desc_t *pInitDesc);

/**
 * \fn bool Example_MainCommandExecute(uint8_t cmdName, example_cmd_payload_t *pCmdPayload)
 * \brief Execute a command
 *
 * \param cmdName name of the command
 * \param pCmdPayload pointer to command payload
 * \return bool: true if operation was a success
 */
bool Example_MainCommandExecute(uint8_t cmdName, example_cmd_payload_t *pCmdPayload);

// *** End Definitions ***
#endif // Example_Main_h
