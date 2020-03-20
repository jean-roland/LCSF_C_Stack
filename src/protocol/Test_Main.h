/**
 * \file Test_Main.h
 * \brief Test protocol module
 * \author LCSF Generator v1.1.0.0
 *
 */

#ifndef test_main_h
#define test_main_h

// *** Libraries include ***
// Standard lib
#include <stdbool.h>
#include <stdint.h>
// Custom lib

// *** Definitions ***
// --- Public Types ---

// Command name enum
enum _test_cmd_names {
    TEST_CMD_SC1,
    TEST_CMD_SC2,
    TEST_CMD_SC3,
    TEST_CMD_CC1,
    TEST_CMD_CC2,
    TEST_CMD_CC3,
    TEST_CMD_CC4,
    TEST_CMD_CC5,
    TEST_CMD_CC6,
    TEST_CMD_COUNT,
};

// Attributes enums
enum _test_ca1_att_names {
    TEST_CA1_ATT_SA1,
    TEST_CA1_ATT_SA2,
    TEST_CA1_ATT_SA3,
};

enum _test_ca10_att_names {
    TEST_CA10_ATT_SA1,
    TEST_CA10_ATT_CA11,
};

enum _test_ca11_att_names {
    TEST_CA11_ATT_SA1,
    TEST_CA11_ATT_CA12,
};

enum _test_ca12_att_names {
    TEST_CA12_ATT_SA4,
};

enum _test_ca2_att_names {
    TEST_CA2_ATT_SA1,
    TEST_CA2_ATT_CA3,
};

enum _test_ca3_att_names {
    TEST_CA3_ATT_SA1,
    TEST_CA3_ATT_CA4,
};

enum _test_ca4_att_names {
    TEST_CA4_ATT_SA4,
};

enum _test_ca5_att_names {
    TEST_CA5_ATT_SA1,
    TEST_CA5_ATT_SA2,
    TEST_CA5_ATT_SA3,
};

enum _test_ca6_att_names {
    TEST_CA6_ATT_SA1,
    TEST_CA6_ATT_CA7,
};

enum _test_ca7_att_names {
    TEST_CA7_ATT_SA1,
    TEST_CA7_ATT_CA8,
};

enum _test_ca8_att_names {
    TEST_CA8_ATT_SA4,
};

enum _test_ca9_att_names {
    TEST_CA9_ATT_SA1,
    TEST_CA9_ATT_SA2,
    TEST_CA9_ATT_SA3,
};

enum _test_cc1_att_names {
    TEST_CC1_ATT_SA1,
    TEST_CC1_ATT_SA2,
    TEST_CC1_ATT_SA3,
    TEST_CC1_ATT_SA4,
    TEST_CC1_ATT_SA5,
    TEST_CC1_ATT_SA6,
    TEST_CC1_ATT_SA7,
    TEST_CC1_ATT_SA8,
    TEST_CC1_ATT_SA9,
    TEST_CC1_ATT_SA10,
};

enum _test_cc2_att_names {
    TEST_CC2_ATT_SA1,
    TEST_CC2_ATT_SA2,
    TEST_CC2_ATT_SA3,
    TEST_CC2_ATT_SA4,
    TEST_CC2_ATT_SA5,
    TEST_CC2_ATT_SA6,
    TEST_CC2_ATT_SA7,
    TEST_CC2_ATT_SA8,
    TEST_CC2_ATT_SA9,
    TEST_CC2_ATT_SA10,
};

enum _test_cc3_att_names {
    TEST_CC3_ATT_SA1,
    TEST_CC3_ATT_SA2,
    TEST_CC3_ATT_SA3,
    TEST_CC3_ATT_SA4,
    TEST_CC3_ATT_SA5,
    TEST_CC3_ATT_SA6,
    TEST_CC3_ATT_SA7,
    TEST_CC3_ATT_SA8,
    TEST_CC3_ATT_SA9,
    TEST_CC3_ATT_SA10,
};

enum _test_cc4_att_names {
    TEST_CC4_ATT_SA1,
    TEST_CC4_ATT_CA1,
    TEST_CC4_ATT_CA2,
};

enum _test_cc5_att_names {
    TEST_CC5_ATT_SA2,
    TEST_CC5_ATT_CA5,
    TEST_CC5_ATT_CA6,
};

enum _test_cc6_att_names {
    TEST_CC6_ATT_SA4,
    TEST_CC6_ATT_CA9,
    TEST_CC6_ATT_CA10,
};

// Attribute with sub-attributes structures
typedef struct _test_ca11_att_ca12_att_payload {
    uint32_t sa4Size;
    uint8_t *p_sa4;
} test_ca11_att_ca12_att_payload_t;

#define TEST_CA11_ATT_SA1_FLAG (1 << 0)
typedef struct _test_ca10_att_ca11_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    test_ca11_att_ca12_att_payload_t ca12_payload;
} test_ca10_att_ca11_att_payload_t;

#define TEST_CA10_ATT_SA1_FLAG (1 << 0)
typedef struct _test_cc6_att_ca10_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    test_ca10_att_ca11_att_payload_t ca11_payload;
} test_cc6_att_ca10_att_payload_t;

#define TEST_CA9_ATT_SA3_FLAG (1 << 0)
typedef struct _test_cc6_att_ca9_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    uint16_t sa2;
    uint32_t sa3;
} test_cc6_att_ca9_att_payload_t;

typedef struct _test_ca7_att_ca8_att_payload {
    uint32_t sa4Size;
    uint8_t *p_sa4;
} test_ca7_att_ca8_att_payload_t;

#define TEST_CA7_ATT_SA1_FLAG (1 << 0)
typedef struct _test_ca6_att_ca7_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    test_ca7_att_ca8_att_payload_t ca8_payload;
} test_ca6_att_ca7_att_payload_t;

#define TEST_CA6_ATT_SA1_FLAG (1 << 0)
typedef struct _test_cc5_att_ca6_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    test_ca6_att_ca7_att_payload_t ca7_payload;
} test_cc5_att_ca6_att_payload_t;

#define TEST_CA5_ATT_SA3_FLAG (1 << 0)
typedef struct _test_cc5_att_ca5_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    uint16_t sa2;
    uint32_t sa3;
} test_cc5_att_ca5_att_payload_t;

typedef struct _test_ca3_att_ca4_att_payload {
    uint32_t sa4Size;
    uint8_t *p_sa4;
} test_ca3_att_ca4_att_payload_t;

#define TEST_CA3_ATT_SA1_FLAG (1 << 0)
typedef struct _test_ca2_att_ca3_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    test_ca3_att_ca4_att_payload_t ca4_payload;
} test_ca2_att_ca3_att_payload_t;

#define TEST_CA2_ATT_SA1_FLAG (1 << 0)
typedef struct _test_cc4_att_ca2_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    test_ca2_att_ca3_att_payload_t ca3_payload;
} test_cc4_att_ca2_att_payload_t;

#define TEST_CA1_ATT_SA3_FLAG (1 << 0)
typedef struct _test_cc4_att_ca1_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    uint16_t sa2;
    uint32_t sa3;
} test_cc4_att_ca1_att_payload_t;

// Commands data structures
#define TEST_CC1_ATT_SA6_FLAG (1 << 0)
#define TEST_CC1_ATT_SA7_FLAG (1 << 1)
#define TEST_CC1_ATT_SA8_FLAG (1 << 2)
#define TEST_CC1_ATT_SA9_FLAG (1 << 3)
#define TEST_CC1_ATT_SA10_FLAG (1 << 4)
typedef struct _test_cc1_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    uint16_t sa2;
    uint32_t sa3;
    uint32_t sa4Size;
    uint8_t *p_sa4;
    char *p_sa5;
    uint8_t sa6;
    uint16_t sa7;
    uint32_t sa8;
    uint32_t sa9Size;
    uint8_t *p_sa9;
    char *p_sa10;
} test_cc1_att_payload_t;

#define TEST_CC2_ATT_SA6_FLAG (1 << 0)
#define TEST_CC2_ATT_SA7_FLAG (1 << 1)
#define TEST_CC2_ATT_SA8_FLAG (1 << 2)
#define TEST_CC2_ATT_SA9_FLAG (1 << 3)
#define TEST_CC2_ATT_SA10_FLAG (1 << 4)
typedef struct _test_cc2_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    uint16_t sa2;
    uint32_t sa3;
    uint32_t sa4Size;
    uint8_t *p_sa4;
    char *p_sa5;
    uint8_t sa6;
    uint16_t sa7;
    uint32_t sa8;
    uint32_t sa9Size;
    uint8_t *p_sa9;
    char *p_sa10;
} test_cc2_att_payload_t;

#define TEST_CC3_ATT_SA6_FLAG (1 << 0)
#define TEST_CC3_ATT_SA7_FLAG (1 << 1)
#define TEST_CC3_ATT_SA8_FLAG (1 << 2)
#define TEST_CC3_ATT_SA9_FLAG (1 << 3)
#define TEST_CC3_ATT_SA10_FLAG (1 << 4)
typedef struct _test_cc3_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    uint16_t sa2;
    uint32_t sa3;
    uint32_t sa4Size;
    uint8_t *p_sa4;
    char *p_sa5;
    uint8_t sa6;
    uint16_t sa7;
    uint32_t sa8;
    uint32_t sa9Size;
    uint8_t *p_sa9;
    char *p_sa10;
} test_cc3_att_payload_t;

#define TEST_CC4_ATT_CA2_FLAG (1 << 0)
typedef struct _test_cc4_att_payload {
    uint8_t optAttFlagsBitfield;
    uint8_t sa1;
    test_cc4_att_ca1_att_payload_t ca1_payload;
    test_cc4_att_ca2_att_payload_t ca2_payload;
} test_cc4_att_payload_t;

#define TEST_CC5_ATT_CA6_FLAG (1 << 0)
typedef struct _test_cc5_att_payload {
    uint8_t optAttFlagsBitfield;
    uint16_t sa2;
    test_cc5_att_ca5_att_payload_t ca5_payload;
    test_cc5_att_ca6_att_payload_t ca6_payload;
} test_cc5_att_payload_t;

#define TEST_CC6_ATT_CA10_FLAG (1 << 0)
typedef struct _test_cc6_att_payload {
    uint8_t optAttFlagsBitfield;
    uint32_t sa4Size;
    uint8_t *p_sa4;
    test_cc6_att_ca9_att_payload_t ca9_payload;
    test_cc6_att_ca10_att_payload_t ca10_payload;
} test_cc6_att_payload_t;

// Command payload union
typedef union _test_cmd_payload {
    test_cc1_att_payload_t cc1_payload;
    test_cc2_att_payload_t cc2_payload;
    test_cc3_att_payload_t cc3_payload;
    test_cc4_att_payload_t cc4_payload;
    test_cc5_att_payload_t cc5_payload;
    test_cc6_att_payload_t cc6_payload;
} test_cmd_payload_t;

// --- Public Function Prototypes ---

/**
 * \fn bool Test_MainInit(void)
 * \brief Initialize the module
 *
 * \return bool: true if operation was a success
 */
bool Test_MainInit(void);

/**
 * \fn void Test_MainClearPattern(const uint8_t *pattern)
 * \brief Set a pattern
 *
 * \param pattern pointer to pattern
 */
void Test_MainSetPattern(const uint8_t *pattern);

/**
 * \fn void Test_MainClearPattern(void)
 * \brief Clear a pattern
 */
void Test_MainClearPattern(void);

/**
 * \fn bool Test_MainExecute(uint16_t cmdName, test_cmd_payload_t *pCmdPayload)
 * \brief Execute a command
 *
 * \param cmdName name of the command
 * \param pCmdPayload pointer to command payload
 * \return bool: true if operation was a success
 */
bool Test_MainExecute(uint16_t cmdName, test_cmd_payload_t *pCmdPayload);

// *** End Definitions ***
#endif // test_Main_h
