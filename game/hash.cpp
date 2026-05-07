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

typedef uint64_t hash_t;

void hash_toHexa(hash_t hash, char* dst) {
	static constexpr char HEX[] = "0123456789abcdef";

	for (int i = 0; i < 16; ++i) {
		const int shift = (15 - i) * 4;
		dst[i] = HEX[(hash >> shift) & 0xF];
	}
}
