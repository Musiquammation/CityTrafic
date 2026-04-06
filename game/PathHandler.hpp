#pragma once

#include "Vector.hpp"
#include "Direction.hpp"
#include <stdint.h>

class PathHandler {
	Vector<int>* array = nullptr;
	uint8_t* dirArray = nullptr;
	int step = -1;
	int length = 0;


public:
	~PathHandler();

	void fill(Vector<int>* array, uint8_t* bitArray, int length);

	/**
	 * @return (x,y) if next point is defined, else (INT_MAX, <?>)
	 */
	Vector<int> seek();

	/**
	 * @warning step is not checked by this function. Use `seek` to get status
	 */
	Direction seekDirection();

	void next();
};
