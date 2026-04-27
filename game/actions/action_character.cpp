#include "action_character.hpp"

#include "ActionNode.hpp"
#include "actionTemplates.hpp"

#include "../Game.hpp"
#include "../Map.hpp"
#include "../Car.hpp"
#include "../Building.hpp"
#include "../Character.hpp"

#include <stdio.h>
#include <cmath>

startns(character)

static inline float frac(float x) {
	return x - std::floor(x);
}

#define LIST(all, fst, run, link)\
	all(result);\
		fst(runDayJob);\
			all(workSection);\
				run(isWorkDay);\
				all(workDay);\
					/* A lot of tasks */ \
			all(chillSection);\
				run(isChillDay);\
				all(chillDay);\
					run(chillDayTest);\
	run(drive);\
	run(walk);\
	run(orientCar);\
	run(orientWork);\
	run(orientHome);\
	run(locateCar);\
	run(locateWork);\
	run(locateHome);\
	run(enter);\
	run(leave);\
	run(passWork);\
			
		



declList();


// Override setData
#undef setData
#define setCharacter() Character* c = (Character*)_data;

struct CharacterFriend {
	def(isWorkDay) {
		return ActionCode::SUCCESS;
	}
	
	def(isChillDay) {
		return ActionCode::FAILURE;
	}

	def(chillDayTest) {
		printf("chillDayTest\n");
		return ActionCode::PENDING;
	}

	def(drive) {
		setCharacter();
		return c->data.drive.state;
	}

	def(walk) {
		setCharacter();
		return c->walk(game);
	}

	def(orientCar) {
		setCharacter();
		if (!c->car)
			return ActionCode::FAILURE;

		bool r = c->makeWalk(game, c->car->x, c->car->y);
		printf("walk %d\n", r);
		return ActionCode_get(r);
	}

	def(orientWork) {
		setCharacter();
		auto& map = game.getMap();
		auto info = c->getWorkBuilding(map);
		bool r = c->orientBuilding(game, info);
		return ActionCode_get(r);
	}

	def(orientHome) {
		setCharacter();
		auto& map = game.getMap();
		auto info = c->getHomeBuilding(map);
		bool r = c->orientBuilding(game, info);
		return ActionCode_get(r);
		return ActionCode::PENDING;
	}

	def(locateCar) {
		printf("locateCar\n");
		return ActionCode::PENDING;
	}

	def(locateWork) {
		printf("locateWork\n");
		return ActionCode::PENDING;
	}

	def(locateHome) {
		printf("locateHome\n");
		return ActionCode::PENDING;
	}

	def(enter) {
		printf("enter\n");
		return ActionCode::PENDING;
	}


	def(leave) {
		setCharacter();

		if (c->state != CharacterState::INSIDE)
			throw std::runtime_error{"Trying to leave while not inside"};


		auto info = game.getMap().getBuilding(
			(int)c->x,
			(int)c->y
		);

		if (!info.building) {
			throw std::runtime_error{"Missing entry building"};
		}

		Vector<int> point;
		{
			int largeLength = info.building->getBufferLargeLength();
			Vector<int> leaveList[largeLength];
			int length = info.building->fillLeaveList(leaveList);
			point = leaveList[c->takeRandomPointId(length)];
		}

		c->x = (float)(info.x + point.x) + .5f;
		c->y = (float)(info.y + point.y) + .5f;

		printf("leave %d %d\n", info.x + point.x, info.y + point.y);

		c->setState(CharacterState::OUTSIDE);
		return ActionCode::SUCCESS;
	}

	def(passWork) {
		printf("passWork\n");
		return ActionCode::PENDING;
	}


	
	
};


give(result, &runDayJob);

give(runDayJob, &workSection, &chillSection);

give(workSection, &isWorkDay, &workDay);

give(workDay,
	&leave,
	&orientCar,
	&walk,
	&locateWork,
	&drive,
	&orientWork,
	&walk,
	&enter,
	&passWork,
	&leave,
	&orientCar,
	&walk,
	&locateHome,
	&drive,
	&orientHome,
	&enter
);

give(chillSection, &isChillDay, &chillDay);

give(chillDay, &chillDayTest);







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

finishns()
