#include "ActionNode.hpp"


namespace actionNodes {
	struct Test {
		static const ActionNode* init();

		int location;
		int time;
	};
}
