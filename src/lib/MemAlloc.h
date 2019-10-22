/**
 * \file MemAlloc.h
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

#ifndef _Mem_Alloc_h
#define _Mem_Alloc_h

//*** Libraries include ***
// Standard lib
#include <stdint.h>
// Custom lib

// *** Definitions ***
// --- Public Types ---
// --- Public Constants ---
// --- Public Variables ---
// --- Public Function Prototypes ---

/**
 * \fn void MemAllocInit(const uint8_t *pHeap, uint32_t heapSize)
 * \brief Heap initialization function, memory will be used for permanent allocation
 *
 * \param pHeap address to the heap (must be 32-bits aligned)
 * \param heapSize heap's size (bytes)
 * \return void
 */
void MemAllocInit(const uint8_t *pHeap, uint32_t heapSize);

/**
 * \fn void *MemAllocMalloc(uint32_t size)
 * \brief Standard memory allocation function
 *
 * \param size size of the memory block (bytes)
 * \return void *: pointer to the allocated memory
 */
void *MemAllocMalloc(uint32_t size);

/**
 * \fn void * MemAllocCalloc(uint32_t size)
 * \brief Standard memory allocation function with initialization to 0
 *
 * \param size size of the memory block (bytes)
 * \return void *: pointer to the allocated memory
 */
void *MemAllocCalloc(uint32_t size);

/**
 * \fn void * MemAllocMallocAligned(uint32_t size, uint8_t alignment)
 * \brief Memory allocation function with alignment (must be multiple of 4)
 *
 * \param size size of the memory block (bytes)
 * \param alignment alignment of the memory block (bits)
 * \return void *: pointer to the allocated memory
 */
void *MemAllocMallocAligned(uint32_t size, uint8_t alignment);

/**
 * \fn void * MemAllocCallocAligned(uint32_t size, uint8_t alignment)
 * \brief Memory allocation function with alignment (must be multiple of 4) and initialization to 0
 *
 * \param size size of the memory block (bytes)
 * \param alignment alignment of the memory block (bits)
 * \return void *: pointer to the allocated memory
 */
void *MemAllocCallocAligned(uint32_t size, uint8_t alignment);

// *** End Definitions ***
#endif // _Mem_Alloc_h