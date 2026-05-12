#include "action_truck.hpp"

#include "actionTemplates.hpp"

#include "../Character.hpp"


#include "../DebugLogger.hpp"
#include "../jobs/TruckImport.hpp"

static DebugLogger print{"ActionTruck", true};


startns(truck)


#define LIST(all, fst, run, loop, link)\
	fst(result)\
		link(leave)\
		loop(main_loop)\
			all(main_all)\
				run(locateClient)\
				link(drive)\
				run(applyClient)\
				run(nextClient)\
		link(enter)\
	run(leave)\
	run(enter)\
	run(drive)\
	run(walk)\
	run(locateWarehouse)\
	run(orientWarehouse)\







declList();

graph(result,
	&leave,
	&main_loop,
	&locateWarehouse,
	&drive,
	&orientWarehouse,
	&walk,
	&enter
);

graph(main_loop,
	&main_all
);

graph(main_all,
	&locateClient,
	&drive,
	&applyClient,
	&nextClient
);



struct CharacterFriend {
	def(leave) {
		setData();
		data->character->makeOutside(game);
		return ActionCode::SUCCESS;
	}

	def(enter) {
		setData();
		bool r = data->character->makeInside(game);
		print("  result=%d\n", r);
		return ActionCode_get(r);
	}



	def(locateClient) {
		setData();
		return ActionCode::SUCCESS;
	}

	def(applyClient) {
		setData();
		return ActionCode::SUCCESS;
	}

	def(nextClient) {
		setData();
		return ActionCode::SUCCESS;
	}

	def(walk) {
		setData();
		return ActionCode::SUCCESS;
	}

	def(drive) {
		setData();
		return ActionCode::SUCCESS;
	}

	def(locateWarehouse) {
		setData();
		return ActionCode::SUCCESS;
	}

	def(orientWarehouse) {
		setData();
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
	TruckImport* targets,
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
	delete[] this->targets;
}


finishns()
