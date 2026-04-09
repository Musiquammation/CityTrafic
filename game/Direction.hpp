#pragma once

#include "Vector.hpp"

#include <stdint.h>
#include <stdexcept>

enum class Direction: uint8_t {
	RIGHT,
	UP,
	LEFT,
	DOWN
};

constexpr Vector<int> DIRECTION_VECTORS[] = {
	{1, 0},  // right
	{0, -1}, // up
	{-1, 0}, // left
	{0, 1}   // down
};

Vector<int> Direction_getVector(Direction dir);
Direction Direction_getRight(Direction dir);
Direction Direction_getLeft(Direction dir);
Direction Direction_getOpposite(Direction dir);
int Direction_getTurn(Direction origin, Direction target);

