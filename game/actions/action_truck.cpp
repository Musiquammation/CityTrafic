#include "action_truck.hpp"

#include "actionTemplates.hpp"

#include "../Character.hpp"


#include "../DebugLogger.hpp"

static DebugLogger print{"ActionTruck", true};


startns(truck)


#define LIST(all, fst, run, loop, link)\
	fst(result)\
		link(leave)\
		loop(main_loop)\
			all(main_all)\
				run(hasOtherCommands)\
		link(enter)\
	run(leave)\
	run(enter)\







declList();

graph(result,
	&leave,
	&main_loop,
	&enter
);

graph(main_loop,
	&main_all
);

graph(main_all,
	&hasOtherCommands
);



struct CharacterFriend {
	def(leave) {
		setData();
		// c->makeOutside(game);
		return ActionCode::SUCCESS;
	}

	def(enter) {
		setData();
		// bool r = c->makeInside(game);
		// return ActionCode_get(r);
		return ActionCode::PENDING;
	}


	def(hasOtherCommands) {
		return ActionCode::SUCCESS;
	}
};


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
