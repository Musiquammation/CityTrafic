#include "action_truck.hpp"

#include "actionTemplates.hpp"

#include "../Game.hpp"
#include "../Map.hpp"
#include "../Character.hpp"


#include "../DebugLogger.hpp"

static DebugLogger print{"ActionTruck", true};


startns(truck)


#define LIST(all, fst, run, link)\
	fst(result)\

struct CharacterFriend {
};



declList();


graph(result,

);


const ActionNode* init() {
	if (initialized)
		return &result;

	initialized = true;

		// Override makeFnc
	#undef makeFnc
	#define makeFnc(name) name = ActionNode{ \
	.type = ActionNodeType::RUNNER, \
	.runner = {CharacterFriend::run_##name} \
	};

	makeList();

	return &result;
}

static void destroyData(void* data) {
	delete (Data*)data;
}

ActionExecutor* createExecutor(
	Character* c,
	Vector<int>* targets,
	int capacity
) {
	auto data = new Data{
		c,
		targets,
		capacity,
		-1
	};

	return new ActionExecutor{
		init(),
		data,
		destroyData
	};
}


Data::~Data() {
	delete this->targets;
}


finishns()
