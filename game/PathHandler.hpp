#pragma once

#include "Vector.hpp"
#include "Direction.hpp"
#include <stdint.h>

template<bool freeArray>
class PathHandler {
	Vector<int>* array = nullptr;
	uint8_t* dirArray = nullptr;
	int step = -1;
	int length = 0;

	template<bool> friend class PathHandler;


public:
	PathHandler() requires (freeArray) = default;

	template<bool freeArraySrc>
	PathHandler(const PathHandler<freeArraySrc>& src);

		
	~PathHandler();

	void fill(Vector<int>* array, uint8_t* bitArray, int length);

	/**
	 * @return (x,y) if next point is defined, else (INT_MAX, INT_MAX)
	 */
	Vector<int> seek();

	/**
	 * @warning step is not checked by this function. Use `seek` to get status
	 */
	Direction seekDirection();

	void next();
};




template class PathHandler<true>;
template class PathHandler<false>;
