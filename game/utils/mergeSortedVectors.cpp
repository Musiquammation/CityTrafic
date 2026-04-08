#include "mergeSortedVectors.hpp"

#include <limits.h>

std::vector<mergeKey_t> mergeSortedVectors(
    const std::vector<mergeKey_t>* vectors,
    int len
) {
	std::vector<mergeKey_t> result;
	std::vector<size_t> indices(len, 0);

	while (true) {
		mergeKey_t chosen{0, INT_MIN};
		int chosenIdx = -1;

		for (int i = 0; i < len; ++i) {
			if (indices[i] < vectors[i].size()) {
				auto e = vectors[i][indices[i]];
				if (chosenIdx == -1 || e.value < chosen.value) {
					chosen = e;
					chosenIdx = i;
				}
			}
		}

		if (chosenIdx == -1) break;

		result.push_back(chosen);
		indices[chosenIdx]++;
	}

	return result;
}
