/**
 * \file Filo.c
 * \brief Filo module
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
    along with this project. If not, see <https://www.gnu.org/licenses/>
 */

// *** Libraries include ***
// Standard lib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Custom lib
#include <LCSF_config.h>
#include <Filo.h>

// *** Definitions ***
// --- Private Types ---
// --- Private Constants ---
// --- Private Function Prototypes ---
// --- Private Variables ---
// *** End Definitions ***

// *** Private Functions ***

// *** Public Functions ***

bool FiloInit(filo_desc_t *pFilo, size_t itemNb, size_t itemSize) {
    if (pFilo == NULL) {
        return false;
    }
    // Filo data array allocation
    pFilo->pDataArray = MEM_ALLOC(itemNb * itemSize);
    // Filo Initialization
    pFilo->ItemSize = itemSize;
    pFilo->ItemNb = itemNb;
    pFilo->FreeItemNb = itemNb;
    return true;
}

bool FiloGet(filo_desc_t *pFiloDesc, size_t itemNb, void **pFreeSlot) {

    if ((pFiloDesc != NULL) && (pFreeSlot != NULL)) {
        // Guard against overflow
        if (itemNb <= pFiloDesc->FreeItemNb) {
            // First available index calculation
            uint32_t slotIndex = pFiloDesc->ItemNb - pFiloDesc->FreeItemNb;
            // Resolve index memory address
            *pFreeSlot = (uint8_t *)pFiloDesc->pDataArray + slotIndex * pFiloDesc->ItemSize;
            // Take allocation into account
            pFiloDesc->FreeItemNb -= itemNb;
            return true;
        } else {
            // Out of memory
            *pFreeSlot = NULL;
        }
    }
    return false;
}

bool FiloFree(filo_desc_t *pFiloDesc, size_t itemNb) {

    if (pFiloDesc != NULL) {
        uint32_t takenSlotNumber = pFiloDesc->ItemNb - pFiloDesc->FreeItemNb;
        // Safety limitation
        if (itemNb > takenSlotNumber) {
            itemNb = takenSlotNumber;
        }
        // Take liberation into account
        pFiloDesc->FreeItemNb += itemNb;
        return true;
    } else {
        return false;
    }
}

bool FiloFreeAll(filo_desc_t *pFiloDesc) {

    if (pFiloDesc != NULL) {
        pFiloDesc->FreeItemNb = pFiloDesc->ItemNb;
        return true;
    } else {
        return false;
    }
}