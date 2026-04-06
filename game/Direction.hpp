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

inline constexpr Vector<int> Direction_getVector(Direction dir) {
	return DIRECTION_VECTORS[(int)dir];
}

constexpr Direction Direction_getRight(Direction dir) {
	switch (dir) {
	case Direction::RIGHT:
		return Direction::DOWN;

	case Direction::UP:
		return Direction::RIGHT;

	case Direction::LEFT:
		return Direction::UP;

	case Direction::DOWN:
		return Direction::LEFT;
	}

	throw std::runtime_error("Invalid Direction");
}

constexpr Direction Direction_getLeft(Direction dir) {
	switch (dir) {
	case Direction::RIGHT:
		return Direction::UP;

	case Direction::UP:
		return Direction::LEFT;

	case Direction::LEFT:
		return Direction::DOWN;

	case Direction::DOWN:
		return Direction::RIGHT;

	};

	throw std::runtime_error("Invalid Direction");
}