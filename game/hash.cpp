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

static constexpr char HEX[] = "0123456789abcdef";
void hash_toHexa(hash_t hash, char* dst) {
	for (int i = 0; i < 16; ++i) {
		const int shift = (15 - i) * 4;
		dst[i] = HEX[(hash >> shift) & 0xF];
	}
}

hash_t hash_toHash(const char* src) {
	hash_t hash = 0;

	for (int i = 0; i < 16 && src[i] != '\0'; ++i) {
		char c = src[i];

		uint8_t value;
		if (c >= '0' && c <= '9') {
			value = (uint8_t)(c - '0');
		}
		else if (c >= 'a' && c <= 'f') {
			value = (uint8_t)(10 + (c - 'a'));
		}
		else if (c >= 'A' && c <= 'F') {
			value = (uint8_t)(10 + (c - 'A'));
		}
		else {
			break;
		}

		hash = (hash << 4) | value;
	}

	return hash;
}
