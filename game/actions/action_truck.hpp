#pragma once
#include "../declarations.hpp"
#include "../Vector.hpp"

class ActionExecutor;

namespace actionNodes {
	namespace truck {
		struct Data {
			Character* c;
			Vector<int>* targets;
			int capacity;
			int current;

			~Data();
		};

		const ActionNode* init();

		ActionExecutor* createExecutor(
			Character* c,
			Vector<int>* targets,
			int capacity
		);
	}
}
