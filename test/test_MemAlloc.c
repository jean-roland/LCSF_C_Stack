#include <stdlib.h>
#include <stdbool.h>
#include "unity.h"
#include "MemAlloc.h"

#define HEAP_SIZE 0x200
#define TEST_LOOP_SIZE 10

static uint8_t _heap[HEAP_SIZE];
static bool init_srand;


static uint32_t pseudo_power(uint8_t exponent) {
    return 1 << exponent;
}

void setUp(void) {
    // Init rand
    if (!init_srand) {
        srand(0x42424242);
        init_srand = true;
    }
    // MemAlloc initialization
    MemAllocInit(_heap, HEAP_SIZE);
}

void tearDown(void) {
}

void test_mem_alloc_no_align(void) {
    // Random block sizes
    uint32_t mallocSize = 1 + rand() % 64;
    uint32_t callocSize = 1 + rand() % 64;
    printf("Test1: mallocSize: %d, callocSize: %d\n", mallocSize, callocSize);
    // Null allocation
    TEST_ASSERT_NULL(MemAllocMalloc(0));
    TEST_ASSERT_NULL(MemAllocCalloc(0));
    // Malloc allocation
    TEST_ASSERT_NOT_NULL(MemAllocMalloc(mallocSize));
    // Calloc allocation
    void *pCallocZone = MemAllocCalloc(callocSize);
    TEST_ASSERT_NOT_NULL(pCallocZone);
    // Check memory initialization
    for (int idx = 0; idx < callocSize; idx++) {
        TEST_ASSERT_TRUE(((uint8_t *)pCallocZone)[idx] == 0);
    }
}

void test_mem_alloc_alignment(void) {
    for (int testIdx = 0; testIdx < TEST_LOOP_SIZE; testIdx++) {
    	MemAllocInit(_heap, HEAP_SIZE);
        // Random block sizes (will allocate all the memory)
        uint32_t mallocSize = 1 + rand() % 64;
        uint32_t callocSize = 1 + rand() % 64;
        // Random alignment
        uint8_t power = 2 + rand() % 6;
        uint8_t alignement = pseudo_power(power);
        printf("Test2: Loop%d: mallocSize: %d, callocSize: %d, alignement: %d\n",
        	testIdx, mallocSize, callocSize, alignement);
        // Null allocation
        TEST_ASSERT_NULL(MemAllocMallocAligned(0, alignement));
        TEST_ASSERT_NULL(MemAllocCallocAligned(0, alignement));
        // Malloc allocation
        void *pMallocZone = MemAllocMallocAligned(mallocSize, alignement);
        TEST_ASSERT_NOT_NULL(pMallocZone);
        TEST_ASSERT_EQUAL_INT(0, ((uintptr_t)pMallocZone & (alignement - 1)));
        // Calloc allocation
        void *pCallocZone = MemAllocCallocAligned(callocSize, alignement);
        TEST_ASSERT_NOT_NULL(pCallocZone);
        TEST_ASSERT_EQUAL_INT(0, ((uintptr_t)pCallocZone & (alignement - 1)));
        // Check memory initialization
        for (int idx = 0; idx < callocSize; idx++) {
            TEST_ASSERT_TRUE(((uint8_t *)pCallocZone)[idx] == 0);
        }
    }
}
