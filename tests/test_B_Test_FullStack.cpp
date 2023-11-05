/**
 * @file test_A_FullStack.cpp
 * @brief Unit test of the whole LCSF stack + Test protocol (B)
 * @author Jean-Roland Gosse
 *
 * This file is part of LCSF C Stack.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>
 */

// *** Private include ***
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <string.h>
#include "Filo.h"
#include "LCSF_Config.h"
#include "LCSF_Transcoder.h"
#include "LCSF_Validator.h"
#include "LCSF_Bridge_Test.h"
#include "Test_Main.h"
}

// *** Private macros ***
#define ARRAY_SIZE 5
#define TX_BUFF_SIZE 255

// *** Private types ***
enum test_id_e {
    TEST_ID_SC2 = 0, // SC2 command test ID
    TEST_ID_SC3, // SC3 command test ID
    TEST_ID_CC2, // CC2 command test id
    TEST_ID_CC3, // CC3 command test id
    TEST_ID_CC5, // CC5 command test id
    TEST_ID_CC6, // CC6 command test id
    TEST_ID_UNKN_PROT, // Unknown protocol test id
    TEST_ID_UNKN_CMD, // Unknown command test id
    TEST_ID_UNKN_ATT, // Unknown attribute test id
    TEST_ID_TM_ATT, // Too many attributes test id
    TEST_ID_MISS_ATT, // Missing attribute test id
    TEST_ID_WR_DATA_TYPE, // Wrong data type test id
    TEST_ID_COUNT, // Test id number
};

// *** Private functions prototypes ***
static void test_buffer(int testid, const uint8_t *pBuffer, size_t buffSize);
static bool senderr_callback(const uint8_t *pBuffer, size_t buffSize);
static void recerr_callback(uint_fast8_t errorLoc, uint_fast8_t errorType);

// *** Private global vars ***
static uint8_t txbuffer[TX_BUFF_SIZE];

// Protocol descriptor
static const lcsf_validator_protocol_desc_t test_prot_desc = {
    LCSF_TEST_PROTOCOL_ID,
    &LCSF_Test_ProtDesc,
    LCSF_Bridge_TestReceive,
};

// *** Model data ***
#ifdef LCSF_SMALL
// Error messages
static const uint8_t err_unknown_prot_msg[] = {0xFF, 0x00, 0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00};
static const uint8_t err_unknown_cmd_msg[] = {0xFF, 0x00, 0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01};
static const uint8_t err_unknown_att_msg[] = {0xFF, 0x00, 0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02};
static const uint8_t err_too_many_att_msg[] = {0xFF, 0x00, 0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03};
static const uint8_t err_missing_att_msg[] = {0xFF, 0x00, 0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x04};
static const uint8_t err_wrong_data_type_msg[] = {0xFF, 0x00, 0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x05};
// Wrong messages
static const uint8_t bad_prot_id_msg[] = {0x21, 0x00, 0x00};
static const uint8_t bad_cmd_id_msg[] = {0x55, 0x4c, 0x00};
static const uint8_t bad_att_id_msg[] = {
    0x55, 0x03, 0x09, // CC1
    0x00, 0x01, 0x00, // SA1
    0x01, 0x02, 0x00, 0x00, // SA2
    0x02, 0x04, 0x00, 0x00, 0x00, 0x00, // SA3
    0x03, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, // SA4
    0x04, 0x01, 0x00, // SA5
    0x0a, 0x05, 0x01, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x04, 0xde, 0x8d, 0x27, 0x40, // SA12
    0x0c, 0x08, 0x75, 0x17, 0x22, 0xaa, 0xfd, 0x90, 0x10, 0x40, // SA13
    0x48, 0x01, 0x00 // Wrong att
};
static const uint8_t extra_att_msg[] = {0x55, 0x00, 0x01, 0x00, 0x01, 0x00};
static const uint8_t missing_att_msg[] = {
    0x55, 0x03, 0x05, // CC1
    0x00, 0x01, 0x00, // SA1
    0x01, 0x02, 0x00, 0x00, // SA2
    0x02, 0x04, 0x00, 0x00, 0x00, 0x00, // SA3
    0x03, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, // SA4
    0x05, 0x01, 0x00 // SA6
};
static const uint8_t bad_data_type_msg[] = {
    0x55, 0x03, 0x05, // CC1
    0x00, 0x04, 0x00, 0x00, 0x00, 0x00, // SA1
    0x01, 0x02, 0x00, 0x00, // SA2
    0x02, 0x04, 0x00, 0x00, 0x00, 0x00, // SA3
    0x03, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, // SA4
    0x04, 0x01, 0x00 // SA5
};
// Test messages
static const uint8_t sc1_msg[] = {0x55, 0x00, 0x00};
static const uint8_t sc2_msg[] = {0x55, 0x01, 0x00};
static const uint8_t sc3_msg[] = {0x55, 0x02, 0x00};
static uint8_t cc1_msg[] = {
    0x55, 0x03, 0x0c, // CC1
    0x00, 0x01, 0x00, // SA1
    0x01, 0x02, 0xD0, 0x07, // SA2
    0x02, 0x04, 0xa0, 0x86, 0x01, 0x00, // SA3
    0x03, 0x05, 0x05, 0x04, 0x03, 0x02, 0x01, // SA4
    0x04, 0x04, 0x42, 0x6f, 0x62, 0x00, // SA5
    0x05, 0x01, 0x03, // SA6
    0x07, 0x04, 0xef, 0x49, 0x02, 0x00, // SA8
    0x08, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, // SA9
    0x09, 0x05, 0x50, 0x61, 0x75, 0x6c, 0x00, // SA10
    0x0a, 0x05, 0x00, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x04, 0xbd, 0x1b, 0xcf, 0x3f, // SA12
    0x0c, 0x08, 0xea, 0x2e, 0x44, 0x54, 0xfb, 0x21, 0x09, 0x40, // SA13
};
static uint8_t cc2_msg[] = {
    0x55, 0x04, 0x0c, // CC2
    0x00, 0x01, 0x01, // SA1
    0x01, 0x02, 0xD1, 0x07, // SA2
    0x02, 0x03, 0xa1, 0x86, 0x01, // SA3
    0x03, 0x05, 0x06, 0x05, 0x04, 0x03, 0x02, // SA4
    0x04, 0x04, 0x43, 0x70, 0x63, 0x00, // SA5
    0x05, 0x01, 0x04, // SA6
    0x07, 0x03, 0xf0, 0x49, 0x02, // SA8
    0x08, 0x05, 0x02, 0x03, 0x04, 0x05, 0x06, // SA9
    0x09, 0x05, 0x51, 0x62, 0x76, 0x6d, 0x00, // SA10
    0x0a, 0x05, 0x01, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x04, 0xde, 0x8d, 0x27, 0x40, // SA12
    0x0c, 0x08, 0x75, 0x17, 0x22, 0xaa, 0xfd, 0x90, 0x10, 0x40, // SA13
};
static uint8_t cc3_msg_rx[] = {
    0x55, 0x05, 0x07, // CC3
    0x00, 0x01, 0x00, // SA1
    0x01, 0x02, 0x00, 0x00, // SA2
    0x02, 0x04, 0xef, 0x49, 0x6a, 0x43, // SA3
    0x03, 0x05, 0x55, 0xaa, 0x55, 0xaa, 0x55, // SA4
    0x04, 0x10, 0x46, 0x6f, 0x77, 0x6c, 0x65, 0x72, 0x2d, 0x4e, 0x6f, 0x72, 0x64, 0x68, 0x65, 0x69, 0x6d, 0x00, // SA5
    0x06, 0x02, 0x00, 0x00, // SA7
    0x08, 0x05, 0x00, 0xff, 0x00, 0xff, 0x00, // SA9
    0x0a, 0x05, 0x01, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x04, 0xde, 0x8d, 0x27, 0x40, // SA12
    0x0c, 0x08, 0x75, 0x17, 0x22, 0xaa, 0xfd, 0x90, 0x10, 0x40, // SA13
};
static uint8_t cc3_msg_tx[] = {
    0x55, 0x05, 0x07, // CC3
    0x00, 0x01, 0xff, // SA1
    0x01, 0x02, 0xff, 0xff, // SA2
    0x02, 0x04, 0xee, 0x49, 0x6a, 0x43, // SA3
    0x03, 0x05, 0x54, 0xa9, 0x54, 0xa9, 0x54, // SA4
    0x04, 0x10, 0x45, 0x6e, 0x76, 0x6b, 0x64, 0x71, 0x2c, 0x4d, 0x6e, 0x71, 0x63, 0x67, 0x64, 0x68, 0x6c, 0x00, // SA5
    0x06, 0x02, 0xff, 0xff, // SA7
    0x08, 0x05, 0xff, 0xfe, 0xff, 0xfe, 0xff, // SA9
    0x0a, 0x05, 0x00, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x04, 0xbd, 0x1b, 0xcf, 0x3f, // SA12
    0x0c, 0x08, 0xea, 0x2e, 0x44, 0x54, 0xfb, 0x21, 0x09, 0x40, // SA13
};
static uint8_t cc4_msg[] = {
    0x55, 0x06, 0x03, // CC4
    0x00, 0x01, 0xff, // SA1
    0x8a, 0x02, // CA1
    0x00, 0x01, 0x23, // CA1_SA1
    0x01, 0x02, 0x0f, 0x20, // CA1_SA2
    0x8b, 0x02, // CA2
    0x00, 0x01, 0xa9, // CA2_SA1
    0x8b, 0x01, // CA2_CA3
    0x8a, 0x01, // CA3_CA4
    0x03, 0x05, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e // CA4_SA4
};
static uint8_t cc5_msg[] = {
    0x55, 0x07, 0x03, // CC5
    0x01, 0x02, 0x00, 0x01, // SA2
    0x8a, 0x02, // CA1
    0x00, 0x01, 0x24, // CA1_SA1
    0x01, 0x02, 0x10, 0x20, // CA1_SA2
    0x8b, 0x02, // CA2
    0x00, 0x01, 0xaa, // CA2_SA1
    0x8b, 0x01, // CA2_CA3
    0x8a, 0x01, // CA3_CA4
    0x03, 0x05, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f // CA4_SA4
};
static uint8_t cc6_msg_rx[] = {
    0x55, 0x08, 0x02, // CC6
    0x03, 0x05, 0x41, 0x02, 0x81, 0x0a, 0xc0, // SA4
    0x8a, 0x03, // CA9
    0x00, 0x01, 0x23, // CA9_SA1
    0x01, 0x02, 0x61, 0x2c, // CA9_SA2
    0x02, 0x04, 0x21, 0x14, 0x00, 0xa2 // CA9_SA3
};
static uint8_t cc6_msg_tx[] = {
    0x55, 0x08, 0x02, // CC6
    0x03, 0x05, 0x40, 0x01, 0x80, 0x09, 0xbf, // SA4
    0x8a, 0x03, // CA9
    0x00, 0x01, 0x22, // CA9_SA1
    0x01, 0x02, 0x60, 0x2c, // CA9_SA2
    0x02, 0x04, 0x20, 0x14, 0x00, 0xa2 // CA9_SA3
};

#else
// Error messages
static const uint8_t err_unknown_prot_msg[] = {
    0xFF, 0xFF, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00
};
static const uint8_t err_unknown_cmd_msg[] = {
    0xFF, 0xFF, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01
};
static const uint8_t err_unknown_att_msg[] = {
    0xFF, 0xFF, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x02
};
static const uint8_t err_too_many_att_msg[] = {
    0xFF, 0xFF, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x03
};
static const uint8_t err_missing_att_msg[] = {
    0xFF, 0xFF, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x04
};
static const uint8_t err_wrong_data_type_msg[] = {
    0xFF, 0xFF, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x05
};
// Wrong messages
static const uint8_t bad_prot_id_msg[] = {0x21, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t bad_cmd_id_msg[] = {0x55, 0x00, 0x4c, 0x00, 0x00, 0x00};
static const uint8_t bad_att_id_msg[] = {
    0x55, 0x00, 0x03, 0x00, 0x09, 0x00, // CC1
    0x00, 0x00, 0x01, 0x00, 0x00, // SA1
    0x01, 0x00, 0x02, 0x00, 0x00, 0x00, // SA2
    0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // SA3
    0x03, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // SA4
    0x04, 0x00, 0x01, 0x00, 0x00, // SA5
    0x0a, 0x00, 0x05, 0x00, 0x01, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x00, 0x04, 0x00, 0xde, 0x8d, 0x27, 0x40, // SA12
    0x0c, 0x00, 0x08, 0x00, 0x75, 0x17, 0x22, 0xaa, 0xfd, 0x90, 0x10, 0x40, // SA13
    0x48, 0x00, 0x01, 0x00, 0x00 // Wrong att
};
static const uint8_t extra_att_msg[] = {0x55, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
static const uint8_t missing_att_msg[] = {
    0x55, 0x00, 0x03, 0x00, 0x05, 0x00, // CC1
    0x00, 0x00, 0x01, 0x00, 0x00, // SA1
    0x01, 0x00, 0x02, 0x00, 0x00, 0x00, // SA2
    0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // SA3
    0x03, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // SA4
    0x05, 0x00, 0x01, 0x00, 0x00 // SA6
};
static const uint8_t bad_data_type_msg[] = {
    0x55, 0x00, 0x03, 0x00, 0x05, 0x00, // CC1
    0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // SA1
    0x01, 0x00, 0x02, 0x00, 0x00, 0x00, // SA2
    0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // SA3
    0x03, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // SA4
    0x04, 0x00, 0x01, 0x00, 0x00 // SA5
};
// Test messages
static const uint8_t sc1_msg[] = {0x55, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t sc2_msg[] = {0x55, 0x00, 0x01, 0x00, 0x00, 0x00};
static const uint8_t sc3_msg[] = {0x55, 0x00, 0x02, 0x00, 0x00, 0x00};
static uint8_t cc1_msg[] = {
    0x55, 0x00, 0x03, 0x00, 0x0c, 0x00, // CC1
    0x00, 0x00, 0x01, 0x00, 0x00, // SA1
    0x01, 0x00, 0x02, 0x00, 0xD0, 0x07, // SA2
    0x02, 0x00, 0x04, 0x00, 0xa0, 0x86, 0x01, 0x00, // SA3
    0x03, 0x00, 0x05, 0x00, 0x05, 0x04, 0x03, 0x02, 0x01, // SA4
    0x04, 0x00, 0x04, 0x00, 0x42, 0x6f, 0x62, 0x00, // SA5
    0x05, 0x00, 0x01, 0x00, 0x03, // SA6
    0x07, 0x00, 0x04, 0x00, 0xef, 0x49, 0x02, 0x00, // SA8
    0x08, 0x00, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, // SA9
    0x09, 0x00, 0x05, 0x00, 0x50, 0x61, 0x75, 0x6c, 0x00, // SA10
    0x0a, 0x00, 0x05, 0x00, 0x00, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x00, 0x04, 0x00, 0xbd, 0x1b, 0xcf, 0x3f, // SA12
    0x0c, 0x00, 0x08, 0x00, 0xea, 0x2e, 0x44, 0x54, 0xfb, 0x21, 0x09, 0x40, // SA13
};
static uint8_t cc2_msg[]= {
    0x55, 0x00, 0x04, 0x00, 0x0c, 0x00, // CC2
    0x00, 0x00, 0x01, 0x00, 0x01, // SA1
    0x01, 0x00, 0x02, 0x00, 0xD1, 0x07, // SA2
    0x02, 0x00, 0x03, 0x00, 0xa1, 0x86, 0x01, // SA3
    0x03, 0x00, 0x05, 0x00, 0x06, 0x05, 0x04, 0x03, 0x02, // SA4
    0x04, 0x00, 0x04, 0x00, 0x43, 0x70, 0x63, 0x00, // SA5
    0x05, 0x00, 0x01, 0x00, 0x04, // SA6
    0x07, 0x00, 0x03, 0x00, 0xf0, 0x49, 0x02, // SA8
    0x08, 0x00, 0x05, 0x00, 0x02, 0x03, 0x04, 0x05, 0x06, // SA9
    0x09, 0x00, 0x05, 0x00, 0x51, 0x62, 0x76, 0x6d, 0x00, // SA10
    0x0a, 0x00, 0x05, 0x00, 0x01, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x00, 0x04, 0x00, 0xde, 0x8d, 0x27, 0x40, // SA12
    0x0c, 0x00, 0x08, 0x00, 0x75, 0x17, 0x22, 0xaa, 0xfd, 0x90, 0x10, 0x40, // SA13
};
static uint8_t cc3_msg_rx[] = {
    0x55, 0x00, 0x05, 0x00, 0x0a, 0x00, // CC3
    0x00, 0x00, 0x01, 0x00, 0x00, // SA1
    0x01, 0x00, 0x02, 0x00, 0x00, 0x00, // SA2
    0x02, 0x00, 0x04, 0x00, 0xef, 0x49, 0x6a, 0x43, // SA3
    0x03, 0x00, 0x05, 0x00, 0x55, 0xaa, 0x55, 0xaa, 0x55, // SA4
    0x04, 0x00, 0x10, 0x00, 0x46, 0x6f, 0x77, 0x6c, 0x65, 0x72, 0x2d, 0x4e, 0x6f, 0x72, 0x64, 0x68, 0x65, 0x69, 0x6d, 0x00, // SA5
    0x06, 0x00, 0x02, 0x00, 0x00, 0x00, // SA7
    0x08, 0x00, 0x05, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, // SA9
    0x0a, 0x00, 0x05, 0x00, 0x00, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x00, 0x04, 0x00, 0xbd, 0x1b, 0xcf, 0x3f, // SA12
    0x0c, 0x00, 0x08, 0x00, 0xea, 0x2e, 0x44, 0x54, 0xfb, 0x21, 0x09, 0x40, // SA13
};
static uint8_t cc3_msg_tx[] = {
    0x55, 0x00, 0x05, 0x00, 0x0a, 0x00, // CC3
    0x00, 0x00, 0x01, 0x00, 0xff, // SA1
    0x01, 0x00, 0x02, 0x00, 0xff, 0xff, // SA2
    0x02, 0x00, 0x04, 0x00, 0xee, 0x49, 0x6a, 0x43, // SA3
    0x03, 0x00, 0x05, 0x00, 0x54, 0xa9, 0x54, 0xa9, 0x54, // SA4
    0x04, 0x00, 0x10, 0x00, 0x45, 0x6e, 0x76, 0x6b, 0x64, 0x71, 0x2c, 0x4d, 0x6e, 0x71, 0x63, 0x67, 0x64, 0x68, 0x6c, 0x00, // SA5
    0x06, 0x00, 0x02, 0x00, 0xff, 0xff, // SA7
    0x08, 0x00, 0x05, 0x00, 0xff, 0xfe, 0xff, 0xfe, 0xff, // SA9
    0x0a, 0x00, 0x05, 0x00, 0x01, 0xf2, 0x05, 0x2a, 0x01, // SA11
    0x0b, 0x00, 0x04, 0x00, 0xde, 0x8d, 0x27, 0x40, // SA12
    0x0c, 0x00, 0x08, 0x00, 0x75, 0x17, 0x22, 0xaa, 0xfd, 0x90, 0x10, 0x40, // SA13
};
static uint8_t cc4_msg[] = {
    0x55, 0x00, 0x06, 0x00, 0x03, 0x00, // CC4
    0x00, 0x00, 0x01, 0x00, 0xff, // SA1
    0x0a, 0x80, 0x02, 0x00, // CA1
    0x00, 0x00, 0x01, 0x00, 0x23, // CA1_SA1
    0x01, 0x00, 0x02, 0x00, 0x0f, 0x20, // CA1_SA2
    0x0b, 0x80, 0x02, 0x00, // CA2
    0x00, 0x00, 0x01, 0x00, 0xa9, // CA2_SA1
    0x0b, 0x80, 0x01, 0x00, // CA2_CA3
    0x0a, 0x80, 0x01, 0x00, // CA3_CA4
    0x03, 0x00, 0x05, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e // CA4_SA4
};
static uint8_t cc5_msg[] = {
    0x55, 0x00, 0x07, 0x00, 0x03, 0x00, // CC5
    0x01, 0x00, 0x02, 0x00, 0x00, 0x01, // SA2
    0x0a, 0x80, 0x02, 0x00, // CA1
    0x00, 0x00, 0x01, 0x00, 0x24, // CA1_SA1
    0x01, 0x00, 0x02, 0x00, 0x10, 0x20, // CA1_SA2
    0x0b, 0x80, 0x02, 0x00, // CA2
    0x00, 0x00, 0x01, 0x00, 0xaa, // CA2_SA1
    0x0b, 0x80, 0x01, 0x00, // CA2_CA3
    0x0a, 0x80, 0x01, 0x00, // CA3_CA4
    0x03, 0x00, 0x05, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f // CA4_SA4
};
static uint8_t cc6_msg_rx[] = {
    0x55, 0x00, 0x08, 0x00, 0x02, 0x00, // CC6
    0x03, 0x00, 0x05, 0x00, 0x41, 0x02, 0x81, 0x0a, 0xc0, // SA4
    0x0a, 0x80, 0x03, 0x00, // CA9
    0x00, 0x00, 0x01, 0x00, 0x23, // CA9_SA1
    0x01, 0x00, 0x02, 0x00, 0x61, 0x2c, // CA9_SA2
    0x02, 0x00, 0x04, 0x00, 0x21, 0x14, 0x00, 0xa2 // CA9_SA3
};
static uint8_t cc6_msg_tx[] = {
    0x55, 0x00, 0x08, 0x00, 0x02, 0x00, // CC6
    0x03, 0x00, 0x05, 0x00, 0x40, 0x01, 0x80, 0x09, 0xbf, // SA4
    0x0a, 0x80, 0x03, 0x00, // CA9
    0x00, 0x00, 0x01, 0x00, 0x22, // CA9_SA1
    0x01, 0x00, 0x02, 0x00, 0x60, 0x2c, // CA9_SA2
    0x02, 0x00, 0x04, 0x00, 0x20, 0x14, 0x00, 0xa2 // CA9_SA3
};
#endif

// *** Private Functions ***

/**
 * @brief      Check the encoded buffer
 *
 * @param[in]  testid    The testid
 * @param[in]  pBuffer   The buffer
 * @param[in]  buffSize  The buffer size
 */
static void test_buffer(int testid, const uint8_t *pBuffer) {
    switch (testid) {
        case TEST_ID_SC2:
            LCSF_DBG_PRINT("[tests]: SC2 test.\n");
            MEMCMP_EQUAL(pBuffer, sc2_msg, sizeof(sc2_msg));
        break;

        case TEST_ID_SC3:
            LCSF_DBG_PRINT("[tests]: SC3 test.\n");
            MEMCMP_EQUAL(pBuffer, sc3_msg, sizeof(sc3_msg));
        break;

        case TEST_ID_CC2:
            LCSF_DBG_PRINT("[tests]: CC2 test.\n");
            MEMCMP_EQUAL(pBuffer, cc2_msg, sizeof(cc2_msg));
        break;

        case TEST_ID_CC3:
            LCSF_DBG_PRINT("[tests]: CC3 test.\n");
            MEMCMP_EQUAL(pBuffer, cc3_msg_tx, sizeof(cc3_msg_tx));
        break;

        case TEST_ID_CC5:
            LCSF_DBG_PRINT("[tests]: CC5 test.\n");
            MEMCMP_EQUAL(pBuffer, cc5_msg, sizeof(cc5_msg));
        break;

        case TEST_ID_CC6:
            LCSF_DBG_PRINT("[tests]: CC6 test.\n");
            MEMCMP_EQUAL(pBuffer, cc6_msg_tx, sizeof(cc6_msg_tx));
        break;

        default:
            LCSF_DBG_PRINT("[tests]: Unknown test!\n");
            CHECK(false);
        break;
    }
}

/**
 * @brief      Function called by LCSF_Validator to send error messages
 *
 * @param[in]  pBuffer   The buffer
 * @param[in]  buffSize  The buffer size
 *
 * @return     bool: true if operation successful
 */
static bool senderr_callback(const uint8_t *pBuffer, size_t buffSize) {
    const uint8_t *pRefBuff = (const uint8_t *) mock().getData("pRefBuff").getPointerValue();
    size_t refBuffSize = mock().getData("refBuffSize").getUnsignedIntValue();
    mock().actualCall("senderr_callback");
    if (buffSize != refBuffSize) {
        LCSF_DBG_PRINT("[tests]: buffer size mismatch %ld %ld\n", buffSize, refBuffSize);
        return false;
    }
    MEMCMP_EQUAL(pBuffer, pRefBuff, refBuffSize);
    return true;
}

/**
 * @brief      Expect a senderr_callback call helper function
 *
 * @param[in]  pRefBuff     The reference buffer
 * @param[in]  refBuffSize  The reference buffer size
 */
static void expect_senderr_cb(const uint8_t *pRefBuff, size_t refBuffSize) {
    mock().setData("pRefBuff", (void *)pRefBuff);
    mock().setData("refBuffSize", (unsigned int) refBuffSize);
    mock().expectOneCall("senderr_callback");
}

// *** Public Functions ***

// *** Tests ***

/**
 * testgroup: B_Test_Fullstack
 *
 * This group tests encoding/decoding of the whole LCSF stack + Test protocol (B)
 */
TEST_GROUP(B_Test_Fullstack) {
    void setup() {
        mock().strictOrder();
        // Init modules
        CHECK(LCSF_TranscoderInit());
        CHECK(LCSF_ValidatorInit(&senderr_callback, NULL));
        CHECK(LCSF_ValidatorAddProtocol(0, &test_prot_desc));
        CHECK(LCSF_Bridge_TestInit());
        CHECK(Test_MainInit(txbuffer, TX_BUFF_SIZE));
    }
    void teardown() {
        mock().checkExpectations();
        mock().clear();
    }
};

/**
 * testgroup: B_Test_Fullstack
 * testname: receive
 *
 * Test the encode/decode of valid messages
 */
TEST(B_Test_Fullstack, valid) {
#ifdef LCSF_SMALL
    LCSF_DBG_PRINT("Smaller LCSF representation is in use.\n");
#else
    LCSF_DBG_PRINT("Regular LCSF representation is in use.\n");
#endif
    // Test function bad command cases
    CHECK(LCSF_TranscoderReceive(sc2_msg, sizeof(sc2_msg)));
    test_buffer(TEST_ID_SC2, txbuffer);
    memset(txbuffer, 0, TX_BUFF_SIZE);
    CHECK(LCSF_TranscoderReceive(cc2_msg, sizeof(cc2_msg)));
    test_buffer(TEST_ID_SC2, txbuffer);
    memset(txbuffer, 0, TX_BUFF_SIZE);
    // Test valid cases
    CHECK(LCSF_TranscoderReceive(sc1_msg, sizeof(sc1_msg)));
    test_buffer(TEST_ID_SC2, txbuffer);
    CHECK(LCSF_TranscoderReceive(sc3_msg, sizeof(sc3_msg)));
    test_buffer(TEST_ID_SC3, txbuffer);
    CHECK(LCSF_TranscoderReceive(cc1_msg, sizeof(cc1_msg)));
    test_buffer(TEST_ID_CC2, txbuffer);
    CHECK(LCSF_TranscoderReceive(cc3_msg_rx, sizeof(cc3_msg_rx)));
    test_buffer(TEST_ID_CC3, txbuffer);
    CHECK(LCSF_TranscoderReceive(cc4_msg, sizeof(cc4_msg)));
    test_buffer(TEST_ID_CC5, txbuffer);
    CHECK(LCSF_TranscoderReceive(cc6_msg_rx, sizeof(cc6_msg_rx)));
    test_buffer(TEST_ID_CC6, txbuffer);
}

/**
 * testgroup: B_Test_Fullstack
 * testname: errors
 *
 * Test the error processing
 */
TEST(B_Test_Fullstack, errors) {
    // Test bad message cases
    expect_senderr_cb(err_unknown_prot_msg, sizeof(err_unknown_prot_msg));
    CHECK_FALSE(LCSF_TranscoderReceive(bad_prot_id_msg, sizeof(bad_prot_id_msg)));
    expect_senderr_cb(err_unknown_cmd_msg, sizeof(err_unknown_cmd_msg));
    CHECK_FALSE(LCSF_TranscoderReceive(bad_cmd_id_msg, sizeof(bad_cmd_id_msg)));
    expect_senderr_cb(err_unknown_att_msg, sizeof(err_unknown_att_msg));
    CHECK_FALSE(LCSF_TranscoderReceive(bad_att_id_msg, sizeof(bad_att_id_msg)));
    expect_senderr_cb(err_too_many_att_msg, sizeof(err_too_many_att_msg));
    CHECK_FALSE(LCSF_TranscoderReceive(extra_att_msg, sizeof(extra_att_msg)));
    expect_senderr_cb(err_missing_att_msg, sizeof(err_missing_att_msg));
    CHECK_FALSE(LCSF_TranscoderReceive(missing_att_msg, sizeof(missing_att_msg)));
    expect_senderr_cb(err_wrong_data_type_msg, sizeof(err_wrong_data_type_msg));
    CHECK_FALSE(LCSF_TranscoderReceive(bad_data_type_msg, sizeof(bad_data_type_msg)));
}