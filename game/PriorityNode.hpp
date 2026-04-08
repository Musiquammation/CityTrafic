#pragma once
#include "declarations.hpp"
#include "Vector.hpp"

struct PriorityNode {
	int frontDist;
	int sideDist;
	int children[3];
	const Car* other;
	Vector<int> targetPoint;
};