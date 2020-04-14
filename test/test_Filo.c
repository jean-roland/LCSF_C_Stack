#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "unity.h"
#include "LCSF_config.h"
#include "Filo.h"
#include "mock_MemAlloc.h"

#define FILO_SIZE 100

static filo_desc_t *pTestFilo;
static bool init_srand;

void setUp(void) {
    // Init rand
    if (!init_srand) {
        srand(0x42424242);
        init_srand = true;
    }
    // Emulate memory allocation
    MemAllocMalloc_ExpectAndReturn(sizeof(filo_desc_t), malloc(sizeof(filo_desc_t)));
    MemAllocMalloc_ExpectAndReturn(FILO_SIZE, malloc(FILO_SIZE));
    // Create filo
    pTestFilo = FiloCreate(FILO_SIZE, sizeof(uint8_t));
    TEST_ASSERT_TRUE_MESSAGE(pTestFilo != NULL, "Couldn't create filo");
}

void tearDown(void) {
    // Free memory allocations
    free(pTestFilo->pDataArray);
    free(pTestFilo);
}

void test_filo_take_free(void) {
    void *pData1, *pData2, *pData3;
    // Allocation tests
    TEST_ASSERT_FALSE(FiloGet(NULL, 0, &pData1));
    TEST_ASSERT_FALSE(FiloGet(pTestFilo, FILO_SIZE + 1, &pData1));
    TEST_ASSERT_NULL(pData1);
    TEST_ASSERT_TRUE(FiloGet(pTestFilo, 0, &pData1));
    TEST_ASSERT_NOT_NULL(pData1);
    int alloc_size = (rand() % FILO_SIZE) + 1;
    int complem_size = FILO_SIZE - alloc_size;
    int small_size = (rand() % 10) + 1;
    printf("size1: %d, size2: %d, size3: %d\n", alloc_size, complem_size, small_size);
    TEST_ASSERT_TRUE(FiloGet(pTestFilo, alloc_size, &pData1));
    TEST_ASSERT_NOT_NULL(pData1);
    TEST_ASSERT_FALSE(FiloGet(pTestFilo, complem_size + 1, &pData2));
    TEST_ASSERT_NULL(pData2);
    TEST_ASSERT_TRUE(FiloGet(pTestFilo, complem_size, &pData2));
    TEST_ASSERT_NOT_NULL(pData2);
    // Free All tests
    TEST_ASSERT_FALSE(FiloFreeAll(NULL));
    TEST_ASSERT_FALSE(FiloGet(pTestFilo, alloc_size, &pData1));
    TEST_ASSERT_NULL(pData1);
    TEST_ASSERT_TRUE(FiloFreeAll(pTestFilo));
    TEST_ASSERT_TRUE(FiloGet(pTestFilo, alloc_size, &pData1));
    TEST_ASSERT_TRUE(FiloGet(pTestFilo, complem_size, &pData2));
    TEST_ASSERT_NOT_NULL(pData1);
    TEST_ASSERT_NOT_NULL(pData2);
    // Free tests
    TEST_ASSERT_FALSE(FiloFree(NULL, 0));
    TEST_ASSERT_TRUE(FiloFree(pTestFilo, 0));
    TEST_ASSERT_FALSE(FiloGet(pTestFilo, small_size, &pData3));
    TEST_ASSERT_NULL(pData3);
    TEST_ASSERT_TRUE(FiloFree(pTestFilo, small_size));
    TEST_ASSERT_TRUE(FiloGet(pTestFilo, small_size, &pData3));
    TEST_ASSERT_NOT_NULL(pData3);
    TEST_ASSERT_TRUE(FiloFree(pTestFilo, 2 * FILO_SIZE));
    TEST_ASSERT_TRUE(FiloGet(pTestFilo, small_size, &pData3));
    TEST_ASSERT_NOT_NULL(pData3);   
    TEST_ASSERT_EQUAL((uintptr_t)pData1, (uintptr_t)pData3);
}