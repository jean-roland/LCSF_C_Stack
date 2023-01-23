/**
 * \file Filo.h
 * \brief Filo module
 * \author Jean-Roland Gosse
 *
 * This file is part of LCSF C Stack.
 *
 * LCSF C Stack is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCSF C Stack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this project. If not, see <https://www.gnu.org/licenses/>
 */

#ifndef _Filo_h
#define _Filo_h

// *** Libraries include ***
// Standard lib
#include <stdbool.h>
#include <stdint.h>
// Custom lib

// *** Definitions ***
// --- Public Types ---
typedef struct _filo_desc {
    void *pDataArray; // Pointer to the filo data array
    uint32_t ItemSize; // Size of the filo item
    uint32_t ItemNb; // Number of filo items
    uint32_t FreeItemNb; // Number of free filo space (item number)
} filo_desc_t;

// --- Public Constants ---
// --- Public Variables ---
// --- Public Function Prototypes ---

/**
 * \fn void FiloInit(filo_desc_t *pFilo, size_t itemNb, size_t itemSize)
 * \brief Initialize a filo
 *
 * \param pFilo: pointer to the filo
 * \param pBuffer: pointer to the filo data buffer (size must be itemNb * itemSize)
 * \param itemNb number of items in the filo
 * \param itemSize size of each item
 * \return bool: true if operation was a success
 */
bool FiloInit(filo_desc_t *pFilo, void *pBuffer, size_t itemNb, size_t itemSize);

/**
 * \fn bool FiloGet(filo_desc_t *pFiloDesc, size_t slotNumber, void **pFreeSlot)
 * \brief Allocate a table of item from a filo and return the pointer
 *
 * \param pFiloDesc pointer to the filo
 * \param itemNb number of items
 * \param pFreeSlot pointer to contain the item table pointer
 * \return bool: true if operation was a success
 */
bool FiloGet(filo_desc_t *pFiloDesc, size_t itemNb, void **pFreeSlot);

/**
 * \fn bool FiloFree(filo_desc_t *pFiloDesc, size_t itemNb)
 * \brief Free items from a filo
 *
 * \param pFiloDesc pointer to the filo
 * \param itemNb number of items
 * \return bool: true if operation was a success
 */
bool FiloFree(filo_desc_t *pFiloDesc, size_t itemNb);

/**
 * \fn bool FiloFreeAll(filo_desc_t *pFiloDesc)
 * \brief Free all items from a filo
 *
 * \param pFiloDesc pointer to the filo
 * \return bool: true if operation was a success
 */
bool FiloFreeAll(filo_desc_t *pFiloDesc);

// *** End Definitions ***
#endif // _Filo_h