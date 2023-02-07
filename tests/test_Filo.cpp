/**
 * @file test_Filo.cpp
 * @brief Unit test of the Filo module
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
#include "LCSF_Config.h"
#include "Filo.h"
}

// *** Private macros ***
#define FILO_SIZE 100

// *** Private variables ***
static filo_desc_t TestFilo;
static uint8_t FiloData[FILO_SIZE];
static bool init_srand;

// *** Public functions ***

/**
 * testgroup: filo
 *
 * This group tests all the filo features
 */
TEST_GROUP(filo) {

    void setup() {
        mock().strictOrder();
        // Init rand
        if (!init_srand) {
            srand(0x42424242);
            init_srand = true;
        }
    }

    void teardown() {
        mock().clear();
    }
};

/**
 * testgroup: filo
 * testname: init
 *
 * Test the FiloInit function
 */
TEST(filo, init) {
    // Null filo pointer
    CHECK_FALSE(FiloInit(NULL, FiloData, FILO_SIZE, sizeof(uint8_t)));

    // Null data pointer
    CHECK_FALSE(FiloInit(&TestFilo, NULL, FILO_SIZE, sizeof(uint8_t)));

    // OK creation
    CHECK(FiloInit(&TestFilo, FiloData, FILO_SIZE, sizeof(uint8_t)));
}

/**
 * testgroup: filo
 * testname: take_free
 *
 * Test the take/free filo features
 */
TEST(filo, take_free) {
    void *pData1, *pData2, *pData3;

    // Create filo
    CHECK_TEXT(FiloInit(&TestFilo, FiloData, FILO_SIZE, sizeof(uint8_t)), "[tests]: Couldn't init filo!");

    // Allocation tests
    CHECK_FALSE(FiloGet(NULL, 0, &pData1));
    CHECK_FALSE(FiloGet(&TestFilo, FILO_SIZE + 1, &pData1));
    CHECK(pData1 == NULL);
    CHECK(FiloGet(&TestFilo, 0, &pData1));
    CHECK(pData1 != NULL);
    int alloc_size = (rand() % FILO_SIZE) + 1;
    int complem_size = FILO_SIZE - alloc_size;
    int small_size = (rand() % 10) + 1;
    LCSF_DBG_PRINT("size1: %d, size2: %d, size3: %d\n", alloc_size, complem_size, small_size);
    CHECK(FiloGet(&TestFilo, alloc_size, &pData1));
    CHECK(pData1 != NULL);
    CHECK_FALSE(FiloGet(&TestFilo, complem_size + 1, &pData2));
    CHECK(pData2 == NULL);
    CHECK(FiloGet(&TestFilo, complem_size, &pData2));
    CHECK(pData2 != NULL);

    // Free All tests
    CHECK_FALSE(FiloFreeAll(NULL));
    CHECK_FALSE(FiloGet(&TestFilo, alloc_size, &pData1));
    CHECK(pData1 == NULL);
    CHECK(FiloFreeAll(&TestFilo));
    CHECK(FiloGet(&TestFilo, alloc_size, &pData1));
    CHECK(FiloGet(&TestFilo, complem_size, &pData2));
    CHECK(pData1 != NULL);
    CHECK(pData2 != NULL);

    // Free tests
    CHECK_FALSE(FiloFree(NULL, 0));
    CHECK(FiloFree(&TestFilo, 0));
    CHECK_FALSE(FiloGet(&TestFilo, small_size, &pData3));
    CHECK(pData3 == NULL);
    CHECK(FiloFree(&TestFilo, small_size));
    CHECK(FiloGet(&TestFilo, small_size, &pData3));
    CHECK(pData3 != NULL);
    CHECK(FiloFree(&TestFilo, 2 * FILO_SIZE));
    CHECK(FiloGet(&TestFilo, small_size, &pData3));
    CHECK(pData3 != NULL);
    CHECK_EQUAL((uintptr_t)pData1, (uintptr_t)pData3);
}