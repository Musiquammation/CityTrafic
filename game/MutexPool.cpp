#include "MutexPool.hpp"



std::shared_mutex& MutexPool::get(MutexLabel label) {
    return mtxs.at((size_t)label);
}

std::shared_lock<std::shared_mutex> MutexPool::lockRead(MutexLabel label) {
    return std::shared_lock<std::shared_mutex>{get(label)};
}

std::unique_lock<std::shared_mutex> MutexPool::lockWrite(MutexLabel label) {
    return std::unique_lock<std::shared_mutex>{get(label)};
}