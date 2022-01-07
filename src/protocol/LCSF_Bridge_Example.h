/**
 * \file LCSF_Bridge_Example.h
 * \brief Example of a LCSF protocol bridge
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

#ifndef LCSF_Bridge_Example_h
#define LCSF_Bridge_Example_h

// *** Libraries include ***
// Standard lib
#include <stdbool.h>
#include <stdint.h>
// Custom lib
#include <LCSF_Validator.h>
#include "Example_Main.h"

// *** Definitions ***
// --- Public Types ---

// Lcsf example protocol identifier
#define LCSF_EXAMPLE_PROTOCOL_ID 0x00

// Lcsf example protocol command identifier enum
enum _lcsf_example_cmd_id {
    LCSF_EXAMPLE_CMD_ID_PING = 0x0,
    LCSF_EXAMPLE_CMD_ID_RESET = 0x1,
    LCSF_EXAMPLE_CMD_ID_ERROR = 0x2,
    LCSF_EXAMPLE_CMD_ID_ACK = 0x3,
    LCSF_EXAMPLE_CMD_ID_ABORT = 0x4,
    LCSF_EXAMPLE_CMD_ID_JUMP = 0x5,
    LCSF_EXAMPLE_CMD_ID_COLOR_SPACE = 0x10,
};

// Lcsf example protocol attribute identifier enum
enum _lcsf_example_att_id {
    LCSF_EXAMPLE_ATT_ERROR_CODE = 0x0,
    LCSF_EXAMPLE_ATT_JUMP_ADDR = 0x1,
    LCSF_EXAMPLE_ATT_YUV = 0x10,
    LCSF_EXAMPLE_ATT_Y = 0x11,
    LCSF_EXAMPLE_ATT_U = 0x12,
    LCSF_EXAMPLE_ATT_V = 0x13,
    LCSF_EXAMPLE_ATT_RGB = 0x20,
    LCSF_EXAMPLE_ATT_R = 0x21,
    LCSF_EXAMPLE_ATT_G = 0x22,
    LCSF_EXAMPLE_ATT_B = 0x23,
};

// --- Public Constants ---

// Lcsf example protocol number of commands
#define LCSF_EXAMPLE_CMD_NB EXAMPLE_CMD_COUNT

// Lcsf example protocol commands attribute number
#define LCSF_EXAMPLE_CMD_ERROR_ATT_NB 1
#define LCSF_EXAMPLE_CMD_JUMP_ATT_NB 1
#define LCSF_EXAMPLE_CMD_COLOR_SPACE_ATT_NB 2
//  Lcsf example protocol attribute sub-attributes number
#define LCSF_EXAMPLE_ATT_YUV_SUBATT_NB 3
#define LCSF_EXAMPLE_ATT_RGB_SUBATT_NB 3

// --- Public Function Prototypes ---
/**
 * \fn bool LCSF_Bridge_ExampleInit(size_t filoSize)
 * \brief Initialize the module
 *
 * \param filoSize size of the module filo (number of element)
 * \return bool: true if operation was a success
 */
bool LCSF_Bridge_ExampleInit(size_t filoSize);

/**
 * \fn bool LCSF_Bridge_ExampleReceive(lcsf_valid_cmd_t *pValidCmd)
 * \brief Receive valid command from LCSF_Validator and transmit to Example_Main
 *
 * \param pValidCmd pointer to the valid command
 * \return bool: true if operation was a success
 */
bool LCSF_Bridge_ExampleReceive(lcsf_valid_cmd_t *pValidCmd);

/**
 * \fn bool LCSF_Bridge_ExampleSend(uint_fast16_t cmdName, void *pCmdPayload)
 * \brief Receive command from Example_Main and transmit to LCSF_Validator
 *
 * \param cmdName name of the command
 * \param pValidCmd pointer to the valid command
 * \return bool: true if operation was a success
 */
bool LCSF_Bridge_ExampleSend(uint_fast16_t cmdName, example_cmd_payload_t *pCmdPayload);

// *** End Definitions ***
#endif // LCSF_Bridge_Example_h
