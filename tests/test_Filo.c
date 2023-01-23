#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "unity.h"
#include "LCSF_config.h"
#include "Filo.h"

#define FILO_SIZE 100

static filo_desc_t TestFilo;
static bool init_srand;

void setUp(void) {
    // Init rand
    if (!init_srand) {
        srand(0x42424242);
        init_srand = true;
    }
    // Emulate memory allocation
    MemAllocMalloc_ExpectAndReturn(FILO_SIZE, malloc(FILO_SIZE));
    // Create filo
    FiloInit(&TestFilo, FILO_SIZE, sizeof(uint8_t));
    TEST_ASSERT_TRUE_MESSAGE(TestFilo.pDataArray != NULL, "Couldn't init filo!");
}

void tearDown(void) {
    // Free memory allocations
    free(TestFilo.pDataArray);
}

void test_filo_take_free(void) {
    void *pData1, *pData2, *pData3;
    // Allocation tests
    TEST_ASSERT_FALSE(FiloGet(NULL, 0, &pData1));
    TEST_ASSERT_FALSE(FiloGet(&TestFilo, FILO_SIZE + 1, &pData1));
    TEST_ASSERT_NULL(pData1);
    TEST_ASSERT_TRUE(FiloGet(&TestFilo, 0, &pData1));
    TEST_ASSERT_NOT_NULL(pData1);
    int alloc_size = (rand() % FILO_SIZE) + 1;
    int complem_size = FILO_SIZE - alloc_size;
    int small_size = (rand() % 10) + 1;
    printf("size1: %d, size2: %d, size3: %d\n", alloc_size, complem_size, small_size);
    TEST_ASSERT_TRUE(FiloGet(&TestFilo, alloc_size, &pData1));
    TEST_ASSERT_NOT_NULL(pData1);
    TEST_ASSERT_FALSE(FiloGet(&TestFilo, complem_size + 1, &pData2));
    TEST_ASSERT_NULL(pData2);
    TEST_ASSERT_TRUE(FiloGet(&TestFilo, complem_size, &pData2));
    TEST_ASSERT_NOT_NULL(pData2);
    // Free All tests
    TEST_ASSERT_FALSE(FiloFreeAll(NULL));
    TEST_ASSERT_FALSE(FiloGet(&TestFilo, alloc_size, &pData1));
    TEST_ASSERT_NULL(pData1);
    TEST_ASSERT_TRUE(FiloFreeAll(&TestFilo));
    TEST_ASSERT_TRUE(FiloGet(&TestFilo, alloc_size, &pData1));
    TEST_ASSERT_TRUE(FiloGet(&TestFilo, complem_size, &pData2));
    TEST_ASSERT_NOT_NULL(pData1);
    TEST_ASSERT_NOT_NULL(pData2);
    // Free tests
    TEST_ASSERT_FALSE(FiloFree(NULL, 0));
    TEST_ASSERT_TRUE(FiloFree(&TestFilo, 0));
    TEST_ASSERT_FALSE(FiloGet(&TestFilo, small_size, &pData3));
    TEST_ASSERT_NULL(pData3);
    TEST_ASSERT_TRUE(FiloFree(&TestFilo, small_size));
    TEST_ASSERT_TRUE(FiloGet(&TestFilo, small_size, &pData3));
    TEST_ASSERT_NOT_NULL(pData3);
    TEST_ASSERT_TRUE(FiloFree(&TestFilo, 2 * FILO_SIZE));
    TEST_ASSERT_TRUE(FiloGet(&TestFilo, small_size, &pData3));
    TEST_ASSERT_NOT_NULL(pData3);   
    TEST_ASSERT_EQUAL((uintptr_t)pData1, (uintptr_t)pData3);
}