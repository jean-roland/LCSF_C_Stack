/**
 * \file LCSF_Config.h
 * \brief Light Command Set Format configuration file
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

#pragma once

// --- LCSF Transcoder ---
// #define LCSF_SMALL // Uncomment to use the smaller representation LCSF variant
#define LCSF_TRANSCODER_RX_FILO_SIZE 20 // Number of items (8 bytes / item)
#define LCSF_TRANSCODER_TX_BUFFER_SIZE 255 // Bytes

// --- LCSF Validator ---
#define LCSF_VALIDATOR_TX_FILO_SIZE 20 // Number of items (8 bytes / item)
#define LCSF_VALIDATOR_RX_FILO_SIZE 20 // Number of items (8 bytes / item)

// Memory allocation include
#include <MemAlloc.h>

// Memory allocation function
#define MEM_ALLOC(size) MemAllocMalloc(size); // Switch to your own function if needed