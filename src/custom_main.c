/**
 * \file custom_main.c
 * \brief Application example for the LCSF C Stack.
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
    along with Network. If not, see <https://www.gnu.org/licenses/>
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include <Filo.h>
#include <LCSF_Transcoder.h>
#include <LCSF_Validator.h>
#include <LCSF_Desc_Example.h>
#include <LCSF_Bridge_Example.h>
#include <Example_Main.h>

// *** Module definitions ***

// Dummy interface
static const uint8_t dummyTxBuff[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static bool dummyReceiveProcess(void) {
    return LCSF_TranscoderReceive(dummyTxBuff, 6);
}

static bool dummyErrSend(const uint8_t *pBuffer, size_t buffSize) {
    if (pBuffer == NULL) {
        return false;
    }
    if (buffSize == 0) {
        return false;
    }
    // Send data
    return true;
}

// --- LCSF Stack ---
enum lcsf_validator_protocol_enum {
    LCSF_PROTOCOL_EXAMPLE = 0,
    LCSF_PROTOCOL_COUNT,
};

static const lcsf_validator_protocol_desc_t LcsfProtocolExampleDesc = {
    LCSF_EXAMPLE_PROTOCOL_ID,
    &LCSF_Example_ProtDesc,
    LCSF_Bridge_ExampleReceive,
};

// --- LCSF Example ---
static const example_init_desc_t ExampleMainInitDesc = {
    0,
};

// *** End of module definitions ***

// Variables

// Functions

/**
 * \fn static void app_init(void)
 * \brief Initialize modules
 *
 * \return void
 */
static void app_init(void) {
    // Lcsf stack
 	LCSF_TranscoderInit();
 	LCSF_ValidatorInit(&dummyErrSend, NULL);
 	LCSF_ValidatorAddProtocol(LCSF_PROTOCOL_EXAMPLE, &LcsfProtocolExampleDesc);
    // Lcsf example
    LCSF_Bridge_ExampleInit();
    Example_MainInit(&ExampleMainInitDesc);
}

/**
 * \fn static void app_process(void)
 * \brief Execute modules process
 *
 * \return void
 */
static void app_process(void) {
    dummyReceiveProcess();
}

/**
 * \fn void main(void)
 * \brief Program entry
 *
* \return int: 0 if successful
 */
int main(void) {
    // Modules initialization
    app_init();

    // Main loop
    while (true) {
        app_process();
    }
    return 0;
}
