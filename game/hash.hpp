#pragma once

#include <array>
#include <stdint.h>

#ifndef HASH_TYPE
#define HASH_TYPE
typedef uint64_t hash_t;
#endif

uint64_t hash_generate();
void hash_longCopy(uint8_t* dst, const uint8_t* src);

#define hash_move(ptr) {ptr += 16;}

