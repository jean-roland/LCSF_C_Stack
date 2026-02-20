/**
 * \file Lifo.c
 * \brief Lifo module
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
#include <lib/Lifo.h>

// *** Definitions ***
// --- Private Types ---
#define MAX_VLE_VALUE_1B 0x00000000000000ff
#define MAX_VLE_VALUE_2B 0x000000000000ffff
#define MAX_VLE_VALUE_3B 0x0000000000ffffff
#define MAX_VLE_VALUE_4B 0x00000000ffffffff
#define MAX_VLE_VALUE_5B 0x000000ffffffffff
#define MAX_VLE_VALUE_6B 0x0000ffffffffffff
#define MAX_VLE_VALUE_7B 0x00ffffffffffffff
// --- Private Constants ---
// --- Private Function Prototypes ---
// --- Private Variables ---
// *** End Definitions ***

// *** Private Functions ***

// *** Public Functions ***

bool LifoInit(lifo_desc_t *pLifo, void *pData, size_t itemNb, size_t itemSize) {
    // Bad parameters guard
    if (pLifo == NULL) {
        return false;
    }
    if (pData == NULL) {
        return false;
    }
    // Lifo Initialization
    pLifo->pDataArray = pData;
    pLifo->ItemSize = itemSize;
    pLifo->ItemNb = itemNb;
    pLifo->FreeItemNb = itemNb;
    return true;
}

bool LifoGet(lifo_desc_t *pLifoDesc, size_t itemNb, void **pFreeSlot) {
    // Bad parameters guard
    if ((pLifoDesc == NULL) || (pFreeSlot == NULL)) {
        return false;
    }
    // Overflow guard
    if (itemNb > pLifoDesc->FreeItemNb) {
        // Out of memory
        *pFreeSlot = NULL;
        return false;
    }
    // First available index calculation
    uint32_t slotIndex = pLifoDesc->ItemNb - pLifoDesc->FreeItemNb;
    // Resolve index memory address
    *pFreeSlot = (uint8_t *)pLifoDesc->pDataArray + slotIndex * pLifoDesc->ItemSize;
    // Take allocation into account
    pLifoDesc->FreeItemNb -= itemNb;
    return true;
}

bool LifoFree(lifo_desc_t *pLifoDesc, size_t itemNb) {
    // Bad parameters guard
    if (pLifoDesc == NULL) {
        return false;
    }
    // Calc number of taken slots
    uint32_t takenSlotNumber = pLifoDesc->ItemNb - pLifoDesc->FreeItemNb;
    // Safety limitation
    if (itemNb > takenSlotNumber) {
        itemNb = takenSlotNumber;
    }
    // Take liberation into account
    pLifoDesc->FreeItemNb += itemNb;
    return true;
}

bool LifoFreeAll(lifo_desc_t *pLifoDesc) {
    // Bad parameters guard
    if (pLifoDesc == NULL) {
        return false;
    }
    // Clear the lifo
    pLifoDesc->FreeItemNb = pLifoDesc->ItemNb;
    return true;
}

uint16_t GetVLESize(uint64_t value) {
    if (value <= MAX_VLE_VALUE_1B) {
        return 1;
    } else if (value <= MAX_VLE_VALUE_2B) {
        return 2;
    } else if (value <= MAX_VLE_VALUE_3B) {
        return 3;
    } else if (value <= MAX_VLE_VALUE_4B) {
        return 4;
    } else if (value <= MAX_VLE_VALUE_5B) {
        return 5;
    } else if (value <= MAX_VLE_VALUE_6B) {
        return 6;
    } else if (value <= MAX_VLE_VALUE_7B) {
        return 7;
    } else {
        return 8;
    }
}
