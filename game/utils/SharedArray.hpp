#pragma once

#include <vector>
#include <shared_mutex>

#include "Array.hpp"

template<typename T>
class SharedArray: public Array<T> {
private:
	std::shared_mutex mtx;

public:
	void take(bool write = false);
	void release(bool write = false);
};