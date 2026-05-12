#pragma once
#include "../declarations.hpp"
#include "../Vector.hpp"

class ActionExecutor;
struct TruckImport;

namespace actionNodes {
	namespace truck {
		struct Data {
			Character* character;
			TruckImport* targets;
			int capacity;
			int current;

			~Data();
		};

		const ActionNode* init();

		ActionExecutor* createExecutor(
			Character* c,
			TruckImport* targets,
			int capacity
		);
	}
}
