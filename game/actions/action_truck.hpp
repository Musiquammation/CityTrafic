#pragma once
#include "../calendar_t.hpp"
#include "../declarations.hpp"
#include "../Vector.hpp"

class ActionExecutor;
struct TruckImport;

namespace actionNodes {
	namespace truck {
		struct Data {
			Character* character;
			TruckImport* targets;
			int length;
			int current;
			int previousFirstImport;
			float units;
			calendar_t finishIndicator;
			Vector<int> warehouse;

			~Data();
		};

		const ActionNode* init();

		ActionExecutor* createExecutor(
			Character* c,
			TruckImport* targets,
			int capacity,
			calendar_t finishIndicator
		);
	}
}
