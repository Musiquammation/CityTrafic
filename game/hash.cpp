#include "hash.hpp"

#include <random>
#include <cstring>

hash_t hash_generate() {
    static thread_local std::mt19937_64 rng(std::random_device{}());
    static thread_local std::uniform_int_distribution<hash_t> dist;

    return dist(rng);
}


void hash_longCopy(uint8_t* dst, const uint8_t* src) {
	memcpy(dst, src, 16);
}

