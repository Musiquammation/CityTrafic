#pragma once
#include "declarations.hpp"
#include "Vector.hpp"

struct PriorityNode {
	int frontDist;
	int sideDist;
	int* children;
	const Car* car;
};
