#include <vector>
#include <functional>

struct mergeKey_t {
    int id;
    int value;
};

std::vector<mergeKey_t> mergeSortedVectors(
    const std::vector<mergeKey_t>* vectors,
    int len
);