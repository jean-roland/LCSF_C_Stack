/**
 * \file MemAlloc.c
 * \brief Memory allocation module
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

//*** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include <MemAlloc.h>

// *** Definitions ***
// --- Private Types ---
typedef struct _memalloc_info {
    const uint8_t *pMemoryHeap; // Pointer to the memory that will be used as the heap
    uint32_t HeapSize; // Size of the heap
    uint32_t MemoryOffset; // Index that keeps track of the heap's usage
} memalloc_info_t;

// --- Private Constants ---
#define MEM_ALLOC_BASE_ALIGNMENT 4

// --- Private Function Prototypes ---
static void *MemAllocGetAddr(uint32_t size, uint8_t alignment);

// --- Private Variables ---
static memalloc_info_t MemAllocInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static void *MemAllocGetAddr(uint32_t size, uint8_t alignment)
 * \brief Function that returns the heap's earliest free aligned memory address (Alignment must be multiple of 4)
 *
 * \param size size of the memory block (bytes)
 * \param alignment alignment of the memory block (bits)
 * \return void *: Pointer to a free aligned memory address
 */
static void *MemAllocGetAddr(uint32_t size, uint8_t alignment) {
    // Size and alignment verification
    if ((size == 0) || ((alignment & 0x3) > 0)) {
        return NULL;
    }
    // 32-bit size alignment
    if ((size & 0x3) > 0) {
        size = (size + 4) & (uint32_t)(~0x3);
    }
    // Find the earliest free aligned memory address
    uintptr_t memAddr = (uintptr_t)MemAllocInfo.pMemoryHeap + MemAllocInfo.MemoryOffset;
    if ((memAddr & (uintptr_t)(alignment - 1)) > 0) {
        memAddr = ((memAddr + alignment) & (uintptr_t)(~(alignment - 1)));
    }
    // Effective size calculation
    uintptr_t alignedSize = memAddr - (uintptr_t)MemAllocInfo.pMemoryHeap - MemAllocInfo.MemoryOffset + size;
    MemAllocInfo.MemoryOffset += (uint32_t)alignedSize;
    // Check if we have enough memory
    if (MemAllocInfo.MemoryOffset > MemAllocInfo.HeapSize) {
        // Out of memory, blocking error
        while (1) {};
    }
    return (void*)memAddr;
}

// *** Public Functions ***

void MemAllocInit(const uint8_t *pHeap, uint32_t heapSize) {
    // Pointer validity and 32-bit alignment test
    if ((pHeap != NULL) && (((uintptr_t)pHeap & 0x3) > 0)) {
        // Address is invalid, blocking error
        while (1) {};
    }
    MemAllocInfo.pMemoryHeap = pHeap;
    MemAllocInfo.HeapSize = heapSize;
    MemAllocInfo.MemoryOffset = 0;
}

void *MemAllocMalloc(uint32_t size) {
    return MemAllocGetAddr(size, MEM_ALLOC_BASE_ALIGNMENT);
}

void *MemAllocCalloc(uint32_t size) {
    void *pData = MemAllocMalloc(size);
    // Check pData validity
    if (pData != NULL) {
        memset(pData, 0, size);
    }
    return pData;
}

void *MemAllocMallocAligned(uint32_t size, uint8_t alignment) {
    return MemAllocGetAddr(size, alignment);
}

void *MemAllocCallocAligned(uint32_t size, uint8_t alignment) {
    void *pData = MemAllocMallocAligned(size, alignment);
    // Check pData validity
    if (pData != NULL) {
        memset(pData, 0, size);
    }
    return pData;
}