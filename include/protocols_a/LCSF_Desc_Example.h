/**
 * \file LCSF_Desc_Example.h
 * \brief Example of a LCSF protocol descriptor
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
#ifndef Lcsf_desc_example_h
#define Lcsf_desc_example_h

// *** Libraries include ***
// Standard lib
#include <stdbool.h>
#include <stdint.h>
// Custom lib
#include "LCSF_Bridge_Example.h"

// *** Definitions ***
// --- Public Constants ---

// Sub-attribute array descriptor of attribute: RGB
const lcsf_attribute_desc_t LCSF_Example_RGB_AttDescArray[LCSF_EXAMPLE_ATT_RGB_SUBATT_NB] = {
    { false, LCSF_UINT16, LCSF_EXAMPLE_ATT_R, 0, NULL },
    { false, LCSF_UINT16, LCSF_EXAMPLE_ATT_G, 0, NULL },
    { false, LCSF_UINT16, LCSF_EXAMPLE_ATT_B, 0, NULL },
};

// Sub-attribute array descriptor of attribute: YUV
const lcsf_attribute_desc_t LCSF_Example_YUV_AttDescArray[LCSF_EXAMPLE_ATT_YUV_SUBATT_NB] = {
    { false, LCSF_UINT16, LCSF_EXAMPLE_ATT_Y, 0, NULL },
    { false, LCSF_UINT16, LCSF_EXAMPLE_ATT_U, 0, NULL },
    { false, LCSF_UINT16, LCSF_EXAMPLE_ATT_V, 0, NULL },
};

// Attribute array descriptor of command: ERROR
const lcsf_attribute_desc_t LCSF_Example_ERROR_AttDescArray[LCSF_EXAMPLE_CMD_ERROR_ATT_NB] = {
    { false, LCSF_UINT8, LCSF_EXAMPLE_ATT_ERROR_CODE, 0, NULL },
};

// Attribute array descriptor of command: JUMP
const lcsf_attribute_desc_t LCSF_Example_JUMP_AttDescArray[LCSF_EXAMPLE_CMD_JUMP_ATT_NB] = {
    { false, LCSF_UINT32, LCSF_EXAMPLE_ATT_JUMP_ADDR, 0, NULL },
};

// Attribute array descriptor of command: COLOR_SPACE
const lcsf_attribute_desc_t LCSF_Example_COLOR_SPACE_AttDescArray[LCSF_EXAMPLE_CMD_COLOR_SPACE_ATT_NB] = {
    { true, LCSF_SUB_ATTRIBUTES, LCSF_EXAMPLE_ATT_YUV, LCSF_EXAMPLE_ATT_YUV_SUBATT_NB, LCSF_Example_YUV_AttDescArray },
    { true, LCSF_SUB_ATTRIBUTES, LCSF_EXAMPLE_ATT_RGB, LCSF_EXAMPLE_ATT_RGB_SUBATT_NB, LCSF_Example_RGB_AttDescArray },
};


// Command array descriptor
const lcsf_command_desc_t LCSF_Example_CmdDescArray[LCSF_EXAMPLE_CMD_NB] = {
    { LCSF_EXAMPLE_CMD_ID_PING, 0, NULL },
    { LCSF_EXAMPLE_CMD_ID_RESET, 0, NULL },
    { LCSF_EXAMPLE_CMD_ID_ERROR, LCSF_EXAMPLE_CMD_ERROR_ATT_NB, LCSF_Example_ERROR_AttDescArray},
    { LCSF_EXAMPLE_CMD_ID_ACK, 0, NULL },
    { LCSF_EXAMPLE_CMD_ID_ABORT, 0, NULL },
    { LCSF_EXAMPLE_CMD_ID_JUMP, LCSF_EXAMPLE_CMD_JUMP_ATT_NB, LCSF_Example_JUMP_AttDescArray},
    { LCSF_EXAMPLE_CMD_ID_COLOR_SPACE, LCSF_EXAMPLE_CMD_COLOR_SPACE_ATT_NB, LCSF_Example_COLOR_SPACE_AttDescArray},
};

// Protocol descriptor
const lcsf_protocol_desc_t LCSF_Example_ProtDesc = { .CmdNb = LCSF_EXAMPLE_CMD_NB, .pCmdDescArray = LCSF_Example_CmdDescArray };

// *** End Definitions ***
#endif // Lcsf_desc_example_h
