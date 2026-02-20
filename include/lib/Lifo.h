/**
 * \file Lifo.h
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

#ifndef _Lifo_h
#define _Lifo_h

// *** Libraries include ***
// Standard lib
// Custom lib
#include <LCSF_Config.h>

// *** Definitions ***
// --- Public Types ---

/** Lifo descriptor structure */
typedef struct _lifo_desc {
    uint16_t ItemSize; /**< Size of the lifo item */
    uint16_t ItemNb; /**< Number of lifo items */
    uint16_t FreeItemNb; /**< Number of free lifo space (item number) */
    void *pDataArray; /**< Pointer to the lifo data array */
} lifo_desc_t; // total: 12 bytes, 2 padding

// --- Public Constants ---
// --- Public Variables ---
// --- Public Function Prototypes ---

/**
 * \fn void LifoInit(lifo_desc_t *pLifo, size_t itemNb, size_t itemSize)
 * \brief Initialize a lifo
 *
 * \param pLifo: pointer to the lifo
 * \param pBuffer: pointer to the lifo data buffer (size must be itemNb * itemSize)
 * \param itemNb number of items in the lifo
 * \param itemSize size of each item
 * \return bool: true if operation was a success
 */
bool LifoInit(lifo_desc_t *pLifo, void *pBuffer, size_t itemNb, size_t itemSize);

/**
 * \fn bool LifoGet(lifo_desc_t *pLifoDesc, size_t slotNumber, void **pFreeSlot)
 * \brief Allocate a table of item from a lifo and return the pointer
 *
 * \param pLifoDesc pointer to the lifo
 * \param itemNb number of items
 * \param pFreeSlot pointer to contain the item table pointer
 * \return bool: true if operation was a success
 */
bool LifoGet(lifo_desc_t *pLifoDesc, size_t itemNb, void **pFreeSlot);

/**
 * \fn bool LifoFree(lifo_desc_t *pLifoDesc, size_t itemNb)
 * \brief Free items from a lifo
 *
 * \param pLifoDesc pointer to the lifo
 * \param itemNb number of items
 * \return bool: true if operation was a success
 */
bool LifoFree(lifo_desc_t *pLifoDesc, size_t itemNb);

/**
 * \fn bool LifoFreeAll(lifo_desc_t *pLifoDesc)
 * \brief Free all items from a lifo
 *
 * \param pLifoDesc pointer to the lifo
 * \return bool: true if operation was a success
 */
bool LifoFreeAll(lifo_desc_t *pLifoDesc);

/**
 * \brief Return byte number to encode value
 *
 * \param value value to encode
 * \return uint16_t: number of bytes to encode value
 */
uint16_t GetVLESize(uint64_t value);

// *** End Definitions ***
#endif // _Lifo_h
