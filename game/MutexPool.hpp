#pragma once

#include <shared_mutex>
#include <mutex>
#include <array>

enum class MutexLabel {
	CARS_STRUCTURE,
	CARS_STATUS,
	CARS_POSITIONS,

	MAP,


	COUNT
};

class MutexPool {
	std::array<std::shared_mutex, (size_t)MutexLabel::COUNT> mtxs;
	std::shared_mutex& get(MutexLabel label);

public:
	MutexPool() = default;

	[[nodiscard("You must keep the lock alive")]]
	std::shared_lock<std::shared_mutex> lockRead(MutexLabel label);

	[[nodiscard("You must keep the lock alive")]]
	std::unique_lock<std::shared_mutex> lockWrite(MutexLabel label);
};