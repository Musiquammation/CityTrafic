#include "Direction.hpp"

Vector<int> Direction_getVector(Direction dir) {
	return DIRECTION_VECTORS[(int)dir];
}

Direction Direction_getRight(Direction dir) {
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

Direction Direction_getLeft(Direction dir) {
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


int Direction_getTurn(Direction origin, Direction target) {
	int diff = ((int)target - (int)origin + 4) % 4;

	if (diff == 0 || diff == 2) return 0;
	if (diff == 1) return -1;
	return 1;

}
