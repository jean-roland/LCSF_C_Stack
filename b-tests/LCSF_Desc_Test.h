/**
 * \file LCSF_Desc_Test.h
 * \brief Test LCSF descriptor
 * \author LCSF Generator v1.1
 *
 */

#ifndef Lcsf_desc_test_h
#define Lcsf_desc_test_h

// *** Libraries include ***
// Standard lib
#include <stdbool.h>
#include <stdint.h>
// Custom lib
#include "LCSF_Bridge_Test.h"

// *** Definitions ***
// --- Public Constants ---

// Sub-attribute array descriptor of attribute CA12
const lcsf_attribute_desc_t LCSF_Test_CA12_AttDescArray[LCSF_TEST_ATT_CA12_SUBATT_NB] = {
    {false, LCSF_BYTE_ARRAY, LCSF_TEST_CA12_ATT_ID_SA4, 0, NULL},
};

// Sub-attribute array descriptor of attribute CA11
const lcsf_attribute_desc_t LCSF_Test_CA11_AttDescArray[LCSF_TEST_ATT_CA11_SUBATT_NB] = {
    {true, LCSF_UINT8, LCSF_TEST_CA11_ATT_ID_SA1, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CA11_ATT_ID_CA12, LCSF_TEST_ATT_CA12_SUBATT_NB, LCSF_Test_CA12_AttDescArray},
};

// Sub-attribute array descriptor of attribute CA10
const lcsf_attribute_desc_t LCSF_Test_CA10_AttDescArray[LCSF_TEST_ATT_CA10_SUBATT_NB] = {
    {true, LCSF_UINT8, LCSF_TEST_CA10_ATT_ID_SA1, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CA10_ATT_ID_CA11, LCSF_TEST_ATT_CA11_SUBATT_NB, LCSF_Test_CA11_AttDescArray},
};

// Sub-attribute array descriptor of attribute CA9
const lcsf_attribute_desc_t LCSF_Test_CA9_AttDescArray[LCSF_TEST_ATT_CA9_SUBATT_NB] = {
    {false, LCSF_UINT8, LCSF_TEST_CA9_ATT_ID_SA1, 0, NULL},
    {false, LCSF_UINT16, LCSF_TEST_CA9_ATT_ID_SA2, 0, NULL},
    {true, LCSF_UINT32, LCSF_TEST_CA9_ATT_ID_SA3, 0, NULL},
};

// Sub-attribute array descriptor of attribute CA8
const lcsf_attribute_desc_t LCSF_Test_CA8_AttDescArray[LCSF_TEST_ATT_CA8_SUBATT_NB] = {
    {false, LCSF_BYTE_ARRAY, LCSF_TEST_CA8_ATT_ID_SA4, 0, NULL},
};

// Sub-attribute array descriptor of attribute CA7
const lcsf_attribute_desc_t LCSF_Test_CA7_AttDescArray[LCSF_TEST_ATT_CA7_SUBATT_NB] = {
    {true, LCSF_UINT8, LCSF_TEST_CA7_ATT_ID_SA1, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CA7_ATT_ID_CA8, LCSF_TEST_ATT_CA8_SUBATT_NB, LCSF_Test_CA8_AttDescArray},
};

// Sub-attribute array descriptor of attribute CA6
const lcsf_attribute_desc_t LCSF_Test_CA6_AttDescArray[LCSF_TEST_ATT_CA6_SUBATT_NB] = {
    {true, LCSF_UINT8, LCSF_TEST_CA6_ATT_ID_SA1, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CA6_ATT_ID_CA7, LCSF_TEST_ATT_CA7_SUBATT_NB, LCSF_Test_CA7_AttDescArray},
};

// Sub-attribute array descriptor of attribute CA5
const lcsf_attribute_desc_t LCSF_Test_CA5_AttDescArray[LCSF_TEST_ATT_CA5_SUBATT_NB] = {
    {false, LCSF_UINT8, LCSF_TEST_CA5_ATT_ID_SA1, 0, NULL},
    {false, LCSF_UINT16, LCSF_TEST_CA5_ATT_ID_SA2, 0, NULL},
    {true, LCSF_UINT32, LCSF_TEST_CA5_ATT_ID_SA3, 0, NULL},
};

// Sub-attribute array descriptor of attribute CA4
const lcsf_attribute_desc_t LCSF_Test_CA4_AttDescArray[LCSF_TEST_ATT_CA4_SUBATT_NB] = {
    {false, LCSF_BYTE_ARRAY, LCSF_TEST_CA4_ATT_ID_SA4, 0, NULL},
};

// Sub-attribute array descriptor of attribute CA3
const lcsf_attribute_desc_t LCSF_Test_CA3_AttDescArray[LCSF_TEST_ATT_CA3_SUBATT_NB] = {
    {true, LCSF_UINT8, LCSF_TEST_CA3_ATT_ID_SA1, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CA3_ATT_ID_CA4, LCSF_TEST_ATT_CA4_SUBATT_NB, LCSF_Test_CA4_AttDescArray},
};

// Sub-attribute array descriptor of attribute CA2
const lcsf_attribute_desc_t LCSF_Test_CA2_AttDescArray[LCSF_TEST_ATT_CA2_SUBATT_NB] = {
    {true, LCSF_UINT8, LCSF_TEST_CA2_ATT_ID_SA1, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CA2_ATT_ID_CA3, LCSF_TEST_ATT_CA3_SUBATT_NB, LCSF_Test_CA3_AttDescArray},
};

// Sub-attribute array descriptor of attribute CA1
const lcsf_attribute_desc_t LCSF_Test_CA1_AttDescArray[LCSF_TEST_ATT_CA1_SUBATT_NB] = {
    {false, LCSF_UINT8, LCSF_TEST_CA1_ATT_ID_SA1, 0, NULL},
    {false, LCSF_UINT16, LCSF_TEST_CA1_ATT_ID_SA2, 0, NULL},
    {true, LCSF_UINT32, LCSF_TEST_CA1_ATT_ID_SA3, 0, NULL},
};

// Attribute array descriptor of command CC1
const lcsf_attribute_desc_t LCSF_Test_CC1_AttDescArray[LCSF_TEST_CMD_CC1_ATT_NB] = {
    {false, LCSF_UINT8, LCSF_TEST_CC1_ATT_ID_SA1, 0, NULL},
    {false, LCSF_UINT16, LCSF_TEST_CC1_ATT_ID_SA2, 0, NULL},
    {false, LCSF_UINT32, LCSF_TEST_CC1_ATT_ID_SA3, 0, NULL},
    {false, LCSF_BYTE_ARRAY, LCSF_TEST_CC1_ATT_ID_SA4, 0, NULL},
    {false, LCSF_STRING, LCSF_TEST_CC1_ATT_ID_SA5, 0, NULL},
    {true, LCSF_UINT8, LCSF_TEST_CC1_ATT_ID_SA6, 0, NULL},
    {true, LCSF_UINT16, LCSF_TEST_CC1_ATT_ID_SA7, 0, NULL},
    {true, LCSF_UINT32, LCSF_TEST_CC1_ATT_ID_SA8, 0, NULL},
    {true, LCSF_BYTE_ARRAY, LCSF_TEST_CC1_ATT_ID_SA9, 0, NULL},
    {true, LCSF_STRING, LCSF_TEST_CC1_ATT_ID_SA10, 0, NULL},
};

// Attribute array descriptor of command CC2
const lcsf_attribute_desc_t LCSF_Test_CC2_AttDescArray[LCSF_TEST_CMD_CC2_ATT_NB] = {
    {false, LCSF_UINT8, LCSF_TEST_CC2_ATT_ID_SA1, 0, NULL},
    {false, LCSF_UINT16, LCSF_TEST_CC2_ATT_ID_SA2, 0, NULL},
    {false, LCSF_UINT32, LCSF_TEST_CC2_ATT_ID_SA3, 0, NULL},
    {false, LCSF_BYTE_ARRAY, LCSF_TEST_CC2_ATT_ID_SA4, 0, NULL},
    {false, LCSF_STRING, LCSF_TEST_CC2_ATT_ID_SA5, 0, NULL},
    {true, LCSF_UINT8, LCSF_TEST_CC2_ATT_ID_SA6, 0, NULL},
    {true, LCSF_UINT16, LCSF_TEST_CC2_ATT_ID_SA7, 0, NULL},
    {true, LCSF_UINT32, LCSF_TEST_CC2_ATT_ID_SA8, 0, NULL},
    {true, LCSF_BYTE_ARRAY, LCSF_TEST_CC2_ATT_ID_SA9, 0, NULL},
    {true, LCSF_STRING, LCSF_TEST_CC2_ATT_ID_SA10, 0, NULL},
};

// Attribute array descriptor of command CC3
const lcsf_attribute_desc_t LCSF_Test_CC3_AttDescArray[LCSF_TEST_CMD_CC3_ATT_NB] = {
    {false, LCSF_UINT8, LCSF_TEST_CC3_ATT_ID_SA1, 0, NULL},
    {false, LCSF_UINT16, LCSF_TEST_CC3_ATT_ID_SA2, 0, NULL},
    {false, LCSF_UINT32, LCSF_TEST_CC3_ATT_ID_SA3, 0, NULL},
    {false, LCSF_BYTE_ARRAY, LCSF_TEST_CC3_ATT_ID_SA4, 0, NULL},
    {false, LCSF_STRING, LCSF_TEST_CC3_ATT_ID_SA5, 0, NULL},
    {true, LCSF_UINT8, LCSF_TEST_CC3_ATT_ID_SA6, 0, NULL},
    {true, LCSF_UINT16, LCSF_TEST_CC3_ATT_ID_SA7, 0, NULL},
    {true, LCSF_UINT32, LCSF_TEST_CC3_ATT_ID_SA8, 0, NULL},
    {true, LCSF_BYTE_ARRAY, LCSF_TEST_CC3_ATT_ID_SA9, 0, NULL},
    {true, LCSF_STRING, LCSF_TEST_CC3_ATT_ID_SA10, 0, NULL},
};

// Attribute array descriptor of command CC4
const lcsf_attribute_desc_t LCSF_Test_CC4_AttDescArray[LCSF_TEST_CMD_CC4_ATT_NB] = {
    {false, LCSF_UINT8, LCSF_TEST_CC4_ATT_ID_SA1, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CC4_ATT_ID_CA1, LCSF_TEST_ATT_CA1_SUBATT_NB, LCSF_Test_CA1_AttDescArray},
    {true, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CC4_ATT_ID_CA2, LCSF_TEST_ATT_CA2_SUBATT_NB, LCSF_Test_CA2_AttDescArray},
};

// Attribute array descriptor of command CC5
const lcsf_attribute_desc_t LCSF_Test_CC5_AttDescArray[LCSF_TEST_CMD_CC5_ATT_NB] = {
    {false, LCSF_UINT16, LCSF_TEST_CC5_ATT_ID_SA2, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CC5_ATT_ID_CA5, LCSF_TEST_ATT_CA5_SUBATT_NB, LCSF_Test_CA5_AttDescArray},
    {true, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CC5_ATT_ID_CA6, LCSF_TEST_ATT_CA6_SUBATT_NB, LCSF_Test_CA6_AttDescArray},
};

// Attribute array descriptor of command CC6
const lcsf_attribute_desc_t LCSF_Test_CC6_AttDescArray[LCSF_TEST_CMD_CC6_ATT_NB] = {
    {false, LCSF_BYTE_ARRAY, LCSF_TEST_CC6_ATT_ID_SA4, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CC6_ATT_ID_CA9, LCSF_TEST_ATT_CA9_SUBATT_NB, LCSF_Test_CA9_AttDescArray},
    {true, LCSF_SUB_ATTRIBUTES, LCSF_TEST_CC6_ATT_ID_CA10, LCSF_TEST_ATT_CA10_SUBATT_NB, LCSF_Test_CA10_AttDescArray},
};

// Command array descriptor
const lcsf_command_desc_t LCSF_Test_CmdDescArray[LCSF_TEST_CMD_NB] = {
    {LCSF_TEST_CMD_ID_SC1, 0, NULL},
    {LCSF_TEST_CMD_ID_SC2, 0, NULL},
    {LCSF_TEST_CMD_ID_SC3, 0, NULL},
    {LCSF_TEST_CMD_ID_CC1, LCSF_TEST_CMD_CC1_ATT_NB, LCSF_Test_CC1_AttDescArray},
    {LCSF_TEST_CMD_ID_CC2, LCSF_TEST_CMD_CC2_ATT_NB, LCSF_Test_CC2_AttDescArray},
    {LCSF_TEST_CMD_ID_CC3, LCSF_TEST_CMD_CC3_ATT_NB, LCSF_Test_CC3_AttDescArray},
    {LCSF_TEST_CMD_ID_CC4, LCSF_TEST_CMD_CC4_ATT_NB, LCSF_Test_CC4_AttDescArray},
    {LCSF_TEST_CMD_ID_CC5, LCSF_TEST_CMD_CC5_ATT_NB, LCSF_Test_CC5_AttDescArray},
    {LCSF_TEST_CMD_ID_CC6, LCSF_TEST_CMD_CC6_ATT_NB, LCSF_Test_CC6_AttDescArray},
};

// Protocol descriptor
const lcsf_protocol_desc_t LCSF_Test_ProtDesc = { .CmdNb = LCSF_TEST_CMD_NB, .pCmdDescArray = LCSF_Test_CmdDescArray };

// *** End Definitions ***
#endif // Lcsf_desc_test_h
