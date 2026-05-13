#include "action_truck.hpp"

#include "actionTemplates.hpp"
#include "../Building.hpp"

#include "../Character.hpp"
#include "../Car.hpp"


#include "../DebugLogger.hpp"
#include "../Game.hpp"
#include "../jobs/TruckImport.hpp"

static DebugLogger print{"ActionTruck", true};
static DebugLogger _printStatus{"TruckStatus", false};
#define printStatus(label, ...) \
_printStatus("[%p] " label, data->character __VA_OPT__(,) __VA_ARGS__)


startns(truck)


#define LIST(all, fst, run, loop, link)\
	all(result)\
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
	run(orientCar)\
	run(walk)\
	run(locateWarehouse)\
	run(orientWarehouse)\







declList();

graph(result,
	&leave,
	&orientCar,
	&walk,
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
	&leave,
	&nextClient
);



struct CharacterFriend {
	def(leave) {
		setData();
		printStatus("leave\n");
		data->character->makeOutside(game);
		return ActionCode::SUCCESS;
	}

	def(enter) {
		setData();
		printStatus("enter\n");
		bool r = data->character->makeInside(game);
		print("  result=%d\n", r);
		return ActionCode_get(r);
	}


	def(orientCar) {
		setData();
		printStatus("orientCar\n");

		auto c = data->character;
		if (!c->car)
			return ActionCode::FAILURE;


		bool r = c->makeWalk(game, c->car->x, c->car->y);
		printStatus("walk %d => %d %d\n", r, c->car->x, c->car->y);
		return ActionCode_get(r);
	}

	def(locateClient) {
		setData();
		printStatus("locateClient\n");
		auto& target = data->targets[data->current];
		auto& map = game.getMap();

		auto info = map.getBuilding(target.x, target.y);
		if (info.building == nullptr)
			return ActionCode::FAILURE;

		bool r = data->character->locateBuilding(map, info);
		return ActionCode_get(r);
	}

	def(applyClient) {
		setData();
		printStatus("applyClient\n");

		auto& target = data->targets[data->current];
		auto& map = game.getMap();
		if (target.id == TruckImportId::DESTINATION) {
			if (auto b = map.getBuilding(target.x, target.y).building) {
				b->truckImport(
					&data->targets[data->previousFirstImport],
					data->units
				);

				data->units = 0; // reset units

			} else {
				return ActionCode::FAILURE;
			}

		} else if (auto b = map.getBuilding(target.x, target.y).building) {
			data->units = b->truckExport(data->units);

		} else {
			return ActionCode::FAILURE;
		}

		return ActionCode::SUCCESS;
	}

	def(nextClient) {
		setData();
		printStatus("nextClient\n");

		if (data->current+1 >= data->length) {
			return ActionCode::FAILURE; // finish loop
		}

		if (data->targets[data->current].id == TruckImportId::DESTINATION) {
			// Check hours
			if (game.getCalendar().indicator >= data->finishIndicator)
				return ActionCode::FAILURE; // finish loop

			// For next client
			data->previousFirstImport = data->current+1;
		}

		data->current++;
		return ActionCode::SUCCESS; // treat next client
	}

	def(walk) {
		setData();
		printStatus("walk\n");
		return data->character->walk(game);
	}

	def(drive) {
		setData();
		printStatus("drive\n");

		auto c = data->character;
		if (c->data.drive.state == ActionCode::PENDING) {
			return ActionCode::PENDING;
		}

		c->setState(CharacterState::OUTSIDE);
		return c->data.drive.state;
	}

	def(locateWarehouse) {
		setData();
		auto& target = data->warehouse;
		printStatus("locateWarehouse target=[%d %d]\n", target.x, target.y);

		auto& map = game.getMap();

		auto info = map.getBuilding(target.x, target.y);
		if (info.building == nullptr) {
			printStatus("  not found\n");
			return ActionCode::FAILURE;
		}

		bool r = data->character->locateBuilding(map, info);
		printStatus("  result %d\n", r);

		return ActionCode_get(r);
	}

	def(orientWarehouse) {
		setData();
		printStatus("orientWarehouse\n");

		auto& target = data->warehouse;
		auto& map = game.getMap();

		auto info = map.getBuilding(target.x, target.y);
		if (info.building == nullptr)
			return ActionCode::FAILURE;

		data->character->orientBuilding(game, info);

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
	int capacity,
	calendar_t finishIndicator
) {
	auto data = new Data{
		c,
		targets,
		capacity,
		0,
		0,
		0.0f,
		finishIndicator,
		{INT32_MIN, INT32_MIN} // will be defined later (by work())
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
