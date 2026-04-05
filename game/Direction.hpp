#pragma once

#include <stdint.h>
#include "Vector.hpp"

enum class Direction: uint8_t {
	RIGHT,
	UP,
	LEFT,
	DOWN
};

constexpr Vector<int> DIRECTION_VECTORS[] = {
	{1, 0},  // right
	{0, -1},  // up
	{-1, 0}, // left
	{0, 1}  // down
};
