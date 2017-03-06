//
// Created by Patrick Nappa on 7/12/16.
//

#pragma once

#include <cstdint>
#include <cstring>

typedef uint32_t rank_t;
typedef uint32_t parent_t;
//block_t stores parent in the upper half, rank in the lower half
typedef uint64_t block_t;

// the max size sparsedomain can store
//typedef size_t sparse_t;

//number of bits each are (sizeof(rank_t) == sizeof(parent_t))
constexpr uint8_t split_size = 32u;
constexpr block_t rank_mask = (2ul << split_size) - 1;

//number of elements in each array of the vector
static constexpr uint8_t BLOCKBITS = 10u;
static constexpr size_t BLOCKSIZE = (1u << BLOCKBITS);

