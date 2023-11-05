/**
 * \file LCSF_Config.h
 * \brief Light Command Set Format configuration file
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

#pragma once

// --- Common include ---
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// --- LCSF Transcoder ---
// #define LCSF_SMALL // Uncomment to use the smaller representation LCSF variant
#define LCSF_TRANSCODER_RX_FILO_SIZE 13 // Number of items (8 bytes / item)

// --- LCSF Validator ---
#define LCSF_VALIDATOR_TX_FILO_SIZE 13 // Number of items (8 bytes / item)
#define LCSF_VALIDATOR_RX_FILO_SIZE 13 // Number of items (8 bytes / item)
#define LCSF_VALIDATOR_PROTOCOL_NB 2

#ifdef _APP_DEBUG
#include <stdio.h>
#define LCSF_DBG_PRINT(...) \
    do { \
        printf(__VA_ARGS__); \
    } while (false)
#else
#define LCSF_DBG_PRINT(...) \
    do { \
    } while (false)
#endif