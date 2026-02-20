/**
 * @file test_Lifo.cpp
 * @brief Unit test of the Lifo module
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
#include "Lifo.h"
}

// *** Private macros ***
#define LIFO_SIZE 100

// *** Private variables ***
static lifo_desc_t TestLifo;
static uint8_t LifoData[LIFO_SIZE];
static bool init_srand;

// *** Public functions ***

/**
 * testgroup: lifo
 *
 * This group tests all the lifo features
 */
TEST_GROUP(lifo) {

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
 * testgroup: lifo
 * testname: init
 *
 * Test the LifoInit function
 */
TEST(lifo, init) {
    // Null lifo pointer
    CHECK_FALSE(LifoInit(NULL, LifoData, LIFO_SIZE, sizeof(uint8_t)));

    // Null data pointer
    CHECK_FALSE(LifoInit(&TestLifo, NULL, LIFO_SIZE, sizeof(uint8_t)));

    // OK creation
    CHECK(LifoInit(&TestLifo, LifoData, LIFO_SIZE, sizeof(uint8_t)));
}

/**
 * testgroup: lifo
 * testname: take_free
 *
 * Test the take/free lifo features
 */
TEST(lifo, take_free) {
    void *pData1, *pData2, *pData3;

    // Create lifo
    CHECK_TEXT(LifoInit(&TestLifo, LifoData, LIFO_SIZE, sizeof(uint8_t)), "[tests]: Couldn't init lifo!");

    // Allocation tests
    CHECK_FALSE(LifoGet(NULL, 0, &pData1));
    CHECK_FALSE(LifoGet(&TestLifo, LIFO_SIZE + 1, &pData1));
    CHECK(pData1 == NULL);
    CHECK(LifoGet(&TestLifo, 0, &pData1));
    CHECK(pData1 != NULL);
    int alloc_size = (rand() % LIFO_SIZE) + 1;
    int complem_size = LIFO_SIZE - alloc_size;
    int small_size = (rand() % 10) + 1;
    LCSF_DBG_PRINT("size1: %d, size2: %d, size3: %d\n", alloc_size, complem_size, small_size);
    CHECK(LifoGet(&TestLifo, alloc_size, &pData1));
    CHECK(pData1 != NULL);
    CHECK_FALSE(LifoGet(&TestLifo, complem_size + 1, &pData2));
    CHECK(pData2 == NULL);
    CHECK(LifoGet(&TestLifo, complem_size, &pData2));
    CHECK(pData2 != NULL);

    // Free All tests
    CHECK_FALSE(LifoFreeAll(NULL));
    CHECK_FALSE(LifoGet(&TestLifo, alloc_size, &pData1));
    CHECK(pData1 == NULL);
    CHECK(LifoFreeAll(&TestLifo));
    CHECK(LifoGet(&TestLifo, alloc_size, &pData1));
    CHECK(LifoGet(&TestLifo, complem_size, &pData2));
    CHECK(pData1 != NULL);
    CHECK(pData2 != NULL);

    // Free tests
    CHECK_FALSE(LifoFree(NULL, 0));
    CHECK(LifoFree(&TestLifo, 0));
    CHECK_FALSE(LifoGet(&TestLifo, small_size, &pData3));
    CHECK(pData3 == NULL);
    CHECK(LifoFree(&TestLifo, small_size));
    CHECK(LifoGet(&TestLifo, small_size, &pData3));
    CHECK(pData3 != NULL);
    CHECK(LifoFree(&TestLifo, 2 * LIFO_SIZE));
    CHECK(LifoGet(&TestLifo, small_size, &pData3));
    CHECK(pData3 != NULL);
    CHECK_EQUAL((uintptr_t)pData1, (uintptr_t)pData3);
}