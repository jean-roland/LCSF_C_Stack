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