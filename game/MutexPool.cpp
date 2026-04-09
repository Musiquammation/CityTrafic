#include "MutexPool.hpp"


void MutexPool::lock(MutexLabel label) {
	size_t idx = static_cast<size_t>(label);
	if (idx >= mtxs.size())
		throw std::out_of_range("Mutex label out of range");

	mtxs[idx].lock();
}

void MutexPool::unlock(MutexLabel label) {
	size_t idx = static_cast<size_t>(label);
	if (idx >= mtxs.size())
		throw std::out_of_range("Mutex label out of range");

	mtxs[idx].unlock();
}
