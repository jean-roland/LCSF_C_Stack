/**
 * \file Filo.c
 * \brief Filo module
 * \author Jean-Roland Gosse
 *
 * This file is part of LCSF C Stack.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include <LCSF_Config.h>
#include <lib/Filo.h>

// *** Definitions ***
// --- Private Types ---
// --- Private Constants ---
// --- Private Function Prototypes ---
// --- Private Variables ---
// *** End Definitions ***

// *** Private Functions ***

// *** Public Functions ***

bool FiloInit(filo_desc_t *pFilo, void *pData, size_t itemNb, size_t itemSize) {
    // Bad parameters guard
    if (pFilo == NULL) {
        return false;
    }
    if (pData == NULL) {
        return false;
    }
    // Filo Initialization
    pFilo->pDataArray = pData;
    pFilo->ItemSize = itemSize;
    pFilo->ItemNb = itemNb;
    pFilo->FreeItemNb = itemNb;
    return true;
}

bool FiloGet(filo_desc_t *pFiloDesc, size_t itemNb, void **pFreeSlot) {
    // Bad parameters guard
    if ((pFiloDesc == NULL) || (pFreeSlot == NULL)) {
        return false;
    }
    // Overflow guard
    if (itemNb > pFiloDesc->FreeItemNb) {
        // Out of memory
        *pFreeSlot = NULL;
        return false;
    }
    // First available index calculation
    uint32_t slotIndex = pFiloDesc->ItemNb - pFiloDesc->FreeItemNb;
    // Resolve index memory address
    *pFreeSlot = (uint8_t *)pFiloDesc->pDataArray + slotIndex * pFiloDesc->ItemSize;
    // Take allocation into account
    pFiloDesc->FreeItemNb -= itemNb;
    return true;
}

bool FiloFree(filo_desc_t *pFiloDesc, size_t itemNb) {
    // Bad parameters guard
    if (pFiloDesc == NULL) {
        return false;
    }
    // Calc number of taken slots
    uint32_t takenSlotNumber = pFiloDesc->ItemNb - pFiloDesc->FreeItemNb;
    // Safety limitation
    if (itemNb > takenSlotNumber) {
        itemNb = takenSlotNumber;
    }
    // Take liberation into account
    pFiloDesc->FreeItemNb += itemNb;
    return true;
}

bool FiloFreeAll(filo_desc_t *pFiloDesc) {
    // Bad parameters guard
    if (pFiloDesc == NULL) {
        return false;
    }
    // Clear the fifo
    pFiloDesc->FreeItemNb = pFiloDesc->ItemNb;
    return true;
}

uint16_t GetVLESize(uint64_t value) {
    if (value <= 0x000000ff) {
        return 1;
    } else if (value <= 0x0000ffff) {
        return 2;
    } else if (value <= 0x00ffffff) {
        return 3;
    } else if (value <= 0xffffffff) {
        return 4;
    } else if (value <= 0x00ffffffffff) {
        return 5;
    } else if (value <= 0x00ffffffffffff) {
        return 6;
    } else if (value <= 0x00ffffffffffffff) {
        return 7;
    } else {
        return 8;
    }
}