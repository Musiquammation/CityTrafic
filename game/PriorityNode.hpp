#pragma once
#include "declarations.hpp"
#include "Vector.hpp"

struct PriorityNode {
	int frontDist;
	int sideDist;
	int childrenLength; 
	const Car* other;
	int* children;
	Vector<int> targetPoint;

	~PriorityNode() {
		free(this->children);
	}
};