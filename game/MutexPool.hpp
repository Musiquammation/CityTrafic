#pragma once

#include <mutex>
#include <array>
#include <stdexcept>

enum class MutexLabel {
	RES_A,
	RES_W,
    
	COUNT
};

class MutexPool {
	std::array<std::mutex, static_cast<size_t>(MutexLabel::COUNT)> mtxs;

public:
	MutexPool() = default;

	void lock(MutexLabel label);

	void unlock(MutexLabel label);

	MutexPool(const MutexPool&) = delete;
	MutexPool& operator=(const MutexPool&) = delete;
};