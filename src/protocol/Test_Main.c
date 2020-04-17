/**
 * \file Test_Main.c
 * \brief Test protocol module (A)
 * \author LCSF Generator v1.1
 *
 */

// *** Libraries include ***
// Standard lib
#include <stdlib.h>
#include <string.h>
// Custom lib
#include <LCSF_config.h>
#include "LCSF_Bridge_Test.h"
#include "Test_Main.h"

// *** Definitions ***
// --- Private Types ---
typedef struct _test_info {
    test_cmd_payload_t *pSendCmdPayload;
    bool hasPattern;
    const uint8_t *pattern;
} test_info_t;

// --- Private Constants ---
#define TEST_ARRAY_SIZE 5

// --- Private Function Prototypes ---
// Generated functions
static bool TestSendCommand(uint16_t cmdName, bool hasPayload);
static bool TestExecuteSC2(void);
static bool TestExecuteSC3(void);
static bool TestExecuteCC2(test_cmd_payload_t *pCmdPayload);
static bool TestExecuteCC3(test_cmd_payload_t *pCmdPayload);
static bool TestExecuteCC5(test_cmd_payload_t *pCmdPayload);
static bool TestExecuteCC6(test_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static test_info_t TestInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static void Test_FillArray(uint8_t *pArray)
 * \brief Fill an array with data
 *
 * \param pArray pointer to array
 * \param incr indicates if function increments or decrements if no pattern
 */
 static void Test_FillArray(uint8_t *pArray, bool incr) {
    if (TestInfo.hasPattern) {
        for (uint8_t idx = 0; idx < TEST_ARRAY_SIZE; idx++) {
            pArray[idx] = TestInfo.pattern[idx];
        }
    } else {
        for (uint8_t idx = 0; idx < TEST_ARRAY_SIZE; idx++) {
            if (incr) {
                pArray[idx] += 1;
            } else {
                pArray[idx] -= 1;
            }
        }
    }
 }

/**
 * \fn static bool TestSendCommand(uint16_t cmdName, bool hasPayload)
 * \brief Send a command
 *=
 * \param cmdName name of the command to send
 * \param hasPayload indicates if command has a payload or not=
 * \return bool: true if operation was a success=
 */
static bool TestSendCommand(uint16_t cmdName, bool hasPayload) {

    if (cmdName >= TEST_CMD_COUNT) {
        return false;
    }
    if (hasPayload) {
        test_cmd_payload_t *pCmdPayload = TestInfo.pSendCmdPayload;
        return LCSF_Bridge_TestSend(cmdName, pCmdPayload);
    } else {
        return LCSF_Bridge_TestSend(cmdName, NULL);
    }
}

/**
 * \fn static bool TestExecuteX(void)
 * \brief Execute command X (no payload)
 *
 * \return bool: true if operation was a success
 */

/**
 * \fn static bool TestExecuteX(test_cmd_payload_t *pCmdPayload)
 * \brief Execute command X (with payload)
 *
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */

static bool TestExecuteSC2(void) {
    return TestSendCommand(TEST_CMD_SC1, false);
}

static bool TestExecuteSC3(void) {
    return TestSendCommand(TEST_CMD_SC3, false);
}

static bool TestExecuteCC2(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
       return false;
    }
    // Declare attributes
    uint8_t m_cc2_sa1 = 0;
    uint16_t m_cc2_sa2 = 0;
    uint32_t m_cc2_sa3 = 0;
    uint8_t *m_cc2_sa4 = NULL;
    char *m_cc2_sa5 = NULL;
    bool m_cc2_sa6_isHere = false;
    uint8_t m_cc2_sa6 = 0;
    bool m_cc2_sa7_isHere = false;
    uint16_t m_cc2_sa7 = 0;
    bool m_cc2_sa8_isHere = false;
    uint32_t m_cc2_sa8 = 0;
    bool m_cc2_sa9_isHere = false;
    uint8_t *m_cc2_sa9 = NULL;
    bool m_cc2_sa10_isHere = false;
    char *m_cc2_sa10 = NULL;
    // Retrieve attributes data
    m_cc2_sa1 = pCmdPayload->cc2_payload.sa1;
    m_cc2_sa2 = pCmdPayload->cc2_payload.sa2;
    m_cc2_sa3 = pCmdPayload->cc2_payload.sa3;
    m_cc2_sa4 = pCmdPayload->cc2_payload.p_sa4;
    m_cc2_sa5 = pCmdPayload->cc2_payload.p_sa5;
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA6_FLAG) != 0) {
        m_cc2_sa6 = pCmdPayload->cc2_payload.sa6;
        m_cc2_sa6_isHere = true;
    }
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA7_FLAG) != 0) {
        m_cc2_sa7 = pCmdPayload->cc2_payload.sa7;
        m_cc2_sa7_isHere = true;
    }
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA8_FLAG) != 0) {
        m_cc2_sa8 = pCmdPayload->cc2_payload.sa8;
        m_cc2_sa8_isHere = true;
    }
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA9_FLAG) != 0) {
        m_cc2_sa9 = pCmdPayload->cc2_payload.p_sa9;
        m_cc2_sa9_isHere = true;
    }
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA10_FLAG) != 0) {
        m_cc2_sa10 = pCmdPayload->cc2_payload.p_sa10;
        m_cc2_sa10_isHere = true;
    }
    // Init payload optAttFlagsBitfield
    TestInfo.pSendCmdPayload->cc1_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.pSendCmdPayload->cc1_payload.sa1 = m_cc2_sa1 + 1;
    TestInfo.pSendCmdPayload->cc1_payload.sa2 = m_cc2_sa2 + 1;
    TestInfo.pSendCmdPayload->cc1_payload.sa3 = m_cc2_sa3 + 1;
    TestInfo.pSendCmdPayload->cc1_payload.sa4Size = TEST_ARRAY_SIZE;
    TestInfo.pSendCmdPayload->cc1_payload.p_sa4 = m_cc2_sa4;
    Test_FillArray(TestInfo.pSendCmdPayload->cc1_payload.p_sa4, true);
    TestInfo.pSendCmdPayload->cc1_payload.p_sa5 = m_cc2_sa5;
    for (uint8_t idx = 0; idx < strlen(m_cc2_sa5); idx++) {
        TestInfo.pSendCmdPayload->cc1_payload.p_sa5[idx] += 1;
    }
    if (m_cc2_sa6_isHere) {
        TestInfo.pSendCmdPayload->cc1_payload.sa6 = m_cc2_sa6 + 1;
        TestInfo.pSendCmdPayload->cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA6_FLAG;
    }
    if (m_cc2_sa7_isHere) {
        TestInfo.pSendCmdPayload->cc1_payload.sa7 = m_cc2_sa7 + 1;
        TestInfo.pSendCmdPayload->cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA7_FLAG;
    }
    if (m_cc2_sa8_isHere) {
        TestInfo.pSendCmdPayload->cc1_payload.sa8 = m_cc2_sa8 + 1;
        TestInfo.pSendCmdPayload->cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA8_FLAG;
    }
    if (m_cc2_sa9_isHere) {
        TestInfo.pSendCmdPayload->cc1_payload.sa9Size = TEST_ARRAY_SIZE;
        TestInfo.pSendCmdPayload->cc1_payload.p_sa9 = m_cc2_sa9;
        Test_FillArray(TestInfo.pSendCmdPayload->cc1_payload.p_sa9, true);
        TestInfo.pSendCmdPayload->cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA9_FLAG;
    }
    if (m_cc2_sa10_isHere) {
        TestInfo.pSendCmdPayload->cc1_payload.p_sa10 = m_cc2_sa10;
        for (uint8_t idx = 0; idx < strlen(m_cc2_sa10); idx++) {
            TestInfo.pSendCmdPayload->cc1_payload.p_sa10[idx] += 1;
        }
        TestInfo.pSendCmdPayload->cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA10_FLAG;
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC1, true);
}

static bool TestExecuteCC3(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
       return false;
    }
    // Declare attributes
    uint8_t m_cc3_sa1 = 0;
    uint16_t m_cc3_sa2 = 0;
    uint32_t m_cc3_sa3 = 0;
    uint8_t *m_cc3_sa4 = NULL;
    char *m_cc3_sa5 = NULL;
    bool m_cc3_sa6_isHere = false;
    uint8_t m_cc3_sa6 = 0;
    bool m_cc3_sa7_isHere = false;
    uint16_t m_cc3_sa7 = 0;
    bool m_cc3_sa8_isHere = false;
    uint32_t m_cc3_sa8 = 0;
    bool m_cc3_sa9_isHere = false;
    uint8_t *m_cc3_sa9 = NULL;
    bool m_cc3_sa10_isHere = false;
    char *m_cc3_sa10 = NULL;
    // Retrieve attributes data
    m_cc3_sa1 = pCmdPayload->cc3_payload.sa1;
    m_cc3_sa2 = pCmdPayload->cc3_payload.sa2;
    m_cc3_sa3 = pCmdPayload->cc3_payload.sa3;
    m_cc3_sa4 = pCmdPayload->cc3_payload.p_sa4;
    m_cc3_sa5 = pCmdPayload->cc3_payload.p_sa5;
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA6_FLAG) != 0) {
        m_cc3_sa6 = pCmdPayload->cc3_payload.sa6;
        m_cc3_sa6_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA7_FLAG) != 0) {
        m_cc3_sa7 = pCmdPayload->cc3_payload.sa7;
        m_cc3_sa7_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA8_FLAG) != 0) {
        m_cc3_sa8 = pCmdPayload->cc3_payload.sa8;
        m_cc3_sa8_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA9_FLAG) != 0) {
        m_cc3_sa9 = pCmdPayload->cc3_payload.p_sa9;
        m_cc3_sa9_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA10_FLAG) != 0) {
        m_cc3_sa10 = pCmdPayload->cc3_payload.p_sa10;
        m_cc3_sa10_isHere = true;
    }
    // Init payload optAttFlagsBitfield
    TestInfo.pSendCmdPayload->cc3_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.pSendCmdPayload->cc3_payload.sa1 = m_cc3_sa1 - 1;
    TestInfo.pSendCmdPayload->cc3_payload.sa2 = m_cc3_sa2 - 1;
    TestInfo.pSendCmdPayload->cc3_payload.sa3 = m_cc3_sa3 - 1;
    TestInfo.pSendCmdPayload->cc3_payload.sa4Size = TEST_ARRAY_SIZE;
    TestInfo.pSendCmdPayload->cc3_payload.p_sa4 = m_cc3_sa4;
    Test_FillArray(TestInfo.pSendCmdPayload->cc3_payload.p_sa4, false);
    TestInfo.pSendCmdPayload->cc3_payload.p_sa5 = m_cc3_sa5;
    for (uint8_t idx = 0; idx < strlen(m_cc3_sa5); idx++) {
        TestInfo.pSendCmdPayload->cc3_payload.p_sa5[idx] -= 1;
    }
    if (m_cc3_sa6_isHere) {
        TestInfo.pSendCmdPayload->cc3_payload.sa6 = m_cc3_sa6 - 1;
        TestInfo.pSendCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA6_FLAG;
    }
    if (m_cc3_sa7_isHere) {
        TestInfo.pSendCmdPayload->cc3_payload.sa7 = m_cc3_sa7 - 1;
        TestInfo.pSendCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA7_FLAG;
    }
    if (m_cc3_sa8_isHere) {
        TestInfo.pSendCmdPayload->cc3_payload.sa8 = m_cc3_sa8 - 1;
        TestInfo.pSendCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA8_FLAG;
    }
    if (m_cc3_sa9_isHere) {
        TestInfo.pSendCmdPayload->cc3_payload.sa9Size = TEST_ARRAY_SIZE;
        TestInfo.pSendCmdPayload->cc3_payload.p_sa9 = m_cc3_sa9;
        Test_FillArray(TestInfo.pSendCmdPayload->cc3_payload.p_sa9, false);
        TestInfo.pSendCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA9_FLAG;
    }
    if (m_cc3_sa10_isHere) {
        TestInfo.pSendCmdPayload->cc3_payload.p_sa10 = m_cc3_sa10;
        for (uint8_t idx = 0; idx < strlen(m_cc3_sa10); idx++) {
            TestInfo.pSendCmdPayload->cc3_payload.p_sa10[idx] -= 1;
        }
        TestInfo.pSendCmdPayload->cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA10_FLAG;
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC3, true);
}

static bool TestExecuteCC5(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
       return false;
    }
    // Declare attributes
    uint16_t m_cc5_sa2 = 0;
    uint8_t m_ca5_sa1 = 0;
    uint16_t m_ca5_sa2 = 0;
    bool m_ca5_sa3_isHere = false;
    uint32_t m_ca5_sa3 = 0;
    bool m_cc5_ca6_isHere = false;
    bool m_ca6_sa1_isHere = false;
    uint8_t m_ca6_sa1 = 0;
    bool m_ca7_sa1_isHere = false;
    uint8_t m_ca7_sa1 = 0;
    uint8_t *m_ca8_sa4 = NULL;
    // Retrieve attributes data
    m_cc5_sa2 = pCmdPayload->cc5_payload.sa2;
    m_ca5_sa1 = pCmdPayload->cc5_payload.ca5_payload.sa1;
    m_ca5_sa2 = pCmdPayload->cc5_payload.ca5_payload.sa2;
    if ((pCmdPayload->cc5_payload.ca5_payload.optAttFlagsBitfield & TEST_CA5_ATT_SA3_FLAG) != 0) {
        m_ca5_sa3 = pCmdPayload->cc5_payload.ca5_payload.sa3;
        m_ca5_sa3_isHere = true;
    }
    if ((pCmdPayload->cc5_payload.optAttFlagsBitfield & TEST_CC5_ATT_CA6_FLAG) != 0) {
        m_cc5_ca6_isHere = true;
        if ((pCmdPayload->cc5_payload.ca6_payload.optAttFlagsBitfield & TEST_CA6_ATT_SA1_FLAG) != 0) {
            m_ca6_sa1 = pCmdPayload->cc5_payload.ca6_payload.sa1;
            m_ca6_sa1_isHere = true;
        }
        if ((pCmdPayload->cc5_payload.ca6_payload.ca7_payload.optAttFlagsBitfield & TEST_CA7_ATT_SA1_FLAG) != 0) {
            m_ca7_sa1 = pCmdPayload->cc5_payload.ca6_payload.ca7_payload.sa1;
            m_ca7_sa1_isHere = true;
        }
        m_ca8_sa4 = pCmdPayload->cc5_payload.ca6_payload.ca7_payload.ca8_payload.p_sa4;
    }
    // Init payload optAttFlagsBitfields
    TestInfo.pSendCmdPayload->cc4_payload.optAttFlagsBitfield = 0;
    TestInfo.pSendCmdPayload->cc4_payload.ca1_payload.optAttFlagsBitfield = 0;
    TestInfo.pSendCmdPayload->cc4_payload.ca2_payload.optAttFlagsBitfield = 0;
    TestInfo.pSendCmdPayload->cc4_payload.ca2_payload.ca3_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.pSendCmdPayload->cc4_payload.sa1 = (uint8_t)m_cc5_sa2 + 1;
    TestInfo.pSendCmdPayload->cc4_payload.ca1_payload.sa1 = m_ca5_sa1 + 1;
    TestInfo.pSendCmdPayload->cc4_payload.ca1_payload.sa2 = m_ca5_sa2 + 1;
    // Optional attributes
    if (m_ca5_sa3_isHere) {
        TestInfo.pSendCmdPayload->cc4_payload.ca1_payload.sa3 = m_ca5_sa3;
        TestInfo.pSendCmdPayload->cc4_payload.ca1_payload.optAttFlagsBitfield |= TEST_CA1_ATT_SA3_FLAG;
    }
    if (m_cc5_ca6_isHere) {
        TestInfo.pSendCmdPayload->cc4_payload.optAttFlagsBitfield |= TEST_CC4_ATT_CA2_FLAG;

        if (m_ca6_sa1_isHere) {
            TestInfo.pSendCmdPayload->cc4_payload.ca2_payload.sa1 = m_ca6_sa1 + 1;
            TestInfo.pSendCmdPayload->cc4_payload.ca2_payload.optAttFlagsBitfield |= TEST_CA2_ATT_SA1_FLAG;
        }
        if (m_ca7_sa1_isHere) {
            TestInfo.pSendCmdPayload->cc4_payload.ca2_payload.ca3_payload.sa1 = m_ca7_sa1 + 1;
            TestInfo.pSendCmdPayload->cc4_payload.ca2_payload.ca3_payload.optAttFlagsBitfield |= TEST_CA3_ATT_SA1_FLAG;
        }
        TestInfo.pSendCmdPayload->cc4_payload.ca2_payload.ca3_payload.ca4_payload.sa4Size = TEST_ARRAY_SIZE;
        TestInfo.pSendCmdPayload->cc4_payload.ca2_payload.ca3_payload.ca4_payload.p_sa4 = m_ca8_sa4;
        Test_FillArray(m_ca8_sa4, true);
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC4, true);
}

static bool TestExecuteCC6(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
       return false;
    }
    // Declare attributes
    uint8_t *m_cc6_sa4 = NULL;
    uint8_t m_ca9_sa1 = 0;
    uint16_t m_ca9_sa2 = 0;
    bool m_ca9_sa3_isHere = false;
    uint32_t m_ca9_sa3 = 0;
    bool m_cc6_ca10_isHere = false;
    bool m_ca10_sa1_isHere = false;
    uint8_t m_ca10_sa1 = 0;
    bool m_ca11_sa1_isHere = false;
    uint8_t m_ca11_sa1 = 0;
    uint8_t *m_ca12_sa4 = NULL;
    // Retrieve attributes data
    m_cc6_sa4 = pCmdPayload->cc6_payload.p_sa4;
    m_ca9_sa1 = pCmdPayload->cc6_payload.ca9_payload.sa1;
    m_ca9_sa2 = pCmdPayload->cc6_payload.ca9_payload.sa2;
    if ((pCmdPayload->cc6_payload.ca9_payload.optAttFlagsBitfield & TEST_CA9_ATT_SA3_FLAG) != 0) {
        m_ca9_sa3 = pCmdPayload->cc6_payload.ca9_payload.sa3;
        m_ca9_sa3_isHere = true;
    }
    if ((pCmdPayload->cc6_payload.optAttFlagsBitfield & TEST_CC6_ATT_CA10_FLAG) != 0) {
        m_cc6_ca10_isHere = true;
        if ((pCmdPayload->cc6_payload.ca10_payload.optAttFlagsBitfield & TEST_CA10_ATT_SA1_FLAG) != 0) {
            m_ca10_sa1 = pCmdPayload->cc6_payload.ca10_payload.sa1;
            m_ca10_sa1_isHere = true;
        }
        if ((pCmdPayload->cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield & TEST_CA11_ATT_SA1_FLAG) != 0) {
            m_ca11_sa1 = pCmdPayload->cc6_payload.ca10_payload.ca11_payload.sa1;
            m_ca11_sa1_isHere = true;
        }
        m_ca12_sa4 = pCmdPayload->cc6_payload.ca10_payload.ca11_payload.ca12_payload.p_sa4;
    }
    // Init payload optAttFlagsBitfields
    TestInfo.pSendCmdPayload->cc6_payload.optAttFlagsBitfield = 0;
    TestInfo.pSendCmdPayload->cc6_payload.ca9_payload.optAttFlagsBitfield = 0;
    TestInfo.pSendCmdPayload->cc6_payload.ca10_payload.optAttFlagsBitfield = 0;
    TestInfo.pSendCmdPayload->cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.pSendCmdPayload->cc6_payload.sa4Size = TEST_ARRAY_SIZE;
    TestInfo.pSendCmdPayload->cc6_payload.p_sa4 = m_cc6_sa4;
    Test_FillArray(m_cc6_sa4, false);
    TestInfo.pSendCmdPayload->cc6_payload.ca9_payload.sa1 = m_ca9_sa1 - 1;
    TestInfo.pSendCmdPayload->cc6_payload.ca9_payload.sa2 = m_ca9_sa2 - 1;
    // Optional attributes
    if (m_ca9_sa3_isHere) {
        TestInfo.pSendCmdPayload->cc6_payload.ca9_payload.sa3 = m_ca9_sa3 - 1;
        TestInfo.pSendCmdPayload->cc6_payload.ca9_payload.optAttFlagsBitfield |= TEST_CA9_ATT_SA3_FLAG;
    }
    if (m_cc6_ca10_isHere) {
        TestInfo.pSendCmdPayload->cc6_payload.optAttFlagsBitfield |= TEST_CC6_ATT_CA10_FLAG;

        if (m_ca10_sa1_isHere) {
            TestInfo.pSendCmdPayload->cc6_payload.ca10_payload.sa1 = m_ca10_sa1 - 1;
            TestInfo.pSendCmdPayload->cc6_payload.ca10_payload.optAttFlagsBitfield |= TEST_CA10_ATT_SA1_FLAG;
        }
        if (m_ca11_sa1_isHere) {
            TestInfo.pSendCmdPayload->cc6_payload.ca10_payload.ca11_payload.sa1 = m_ca11_sa1 - 1;
            TestInfo.pSendCmdPayload->cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield |= TEST_CA11_ATT_SA1_FLAG;
        }
        TestInfo.pSendCmdPayload->cc6_payload.ca10_payload.ca11_payload.ca12_payload.sa4Size = TEST_ARRAY_SIZE;
        TestInfo.pSendCmdPayload->cc6_payload.ca10_payload.ca11_payload.ca12_payload.p_sa4 = m_ca12_sa4;
        Test_FillArray(m_ca12_sa4, false);
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC6, true);
}

// *** Public Functions ***

/**
 * \fn bool Test_MainInit(void)
 * \brief Initialize the module
 *
 * \return bool: true if operation was a success
 */
bool Test_MainInit(void) {
    TestInfo.pSendCmdPayload = (test_cmd_payload_t *)MEM_ALLOC(sizeof(test_cmd_payload_t));
    return true;
}

/**
 * \fn void Test_MainClearPattern(const uint8_t *pattern)
 * \brief Set a pattern
 *
 * \param pattern pointer to pattern
 */
void Test_MainSetPattern(const uint8_t *pattern) {
    if (pattern == NULL) {
        return;
    }
    TestInfo.hasPattern = true;
    TestInfo.pattern = pattern;
}

/**
 * \fn void Test_MainClearPattern(void)
 * \brief Clear a pattern
 */
void Test_MainClearPattern(void) {
    TestInfo.hasPattern = false;
    TestInfo.pattern = NULL;
}

/**
 * \fn bool Test_MainExecute(uint16_t cmdName, test_cmd_payload_t *pCmdPayload)
 * \brief Execute a command
 *
 * \param cmdName name of the command
 * \param pCmdPayload pointer to command payload
 * \return bool: true if operation was a success
 */
bool Test_MainExecute(uint16_t cmdName, test_cmd_payload_t *pCmdPayload) {
    switch (cmdName) {
        case TEST_CMD_SC2:
            return TestExecuteSC2();
        break;

        case TEST_CMD_SC3:
            return TestExecuteSC3();
        break;

        case TEST_CMD_CC2:
            return TestExecuteCC2(pCmdPayload);
        break;

        case TEST_CMD_CC3:
            return TestExecuteCC3(pCmdPayload);
        break;

        case TEST_CMD_CC5:
            return TestExecuteCC5(pCmdPayload);
        break;

        case TEST_CMD_CC6:
            return TestExecuteCC6(pCmdPayload);
        break;

        default:
            return false;
        break;
    }
}
