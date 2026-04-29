#include "action_character.hpp"

#include "ActionNode.hpp"
#include "actionTemplates.hpp"

#include "../Game.hpp"
#include "../Map.hpp"
#include "../Car.hpp"
#include "../Job.hpp"
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
				run(mustWork);\
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
	run(locateWork);\
	run(locateHome);\
	run(enter);\
	run(leave);\
	run(enterWork);\
	run(leaveWork);\
	run(passWork);\
			
		



declList();


// Override setData
#undef setData
#define setCharacter() Character* c = (Character*)_data;

struct CharacterFriend {
	def(mustWork) {
		setCharacter();
		Job* job = c->getJob();
		if (!job) {
			// No job so no need to work
			return ActionCode::FAILURE;
		}

		auto& calendar = game.getCalendar();
		auto hour = job->getNextEnterHour(c, calendar);

		return ActionCode_get(calendar.indicator > hour);
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
		if (c->data.drive.state == ActionCode::PENDING) {
			return ActionCode::PENDING;
		}

		c->setState(CharacterState::OUTSIDE);
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
		printf("walk %d => %d %d\n", r, c->car->x, c->car->y);
		return ActionCode_get(r);
	}

	def(orientWork) {
		setCharacter();
		if (!c->getJob())
			return ActionCode::FAILURE; // no job
			
		auto info = c->getWorkBuilding(game);
		bool r = c->orientBuilding(game, info);
		return ActionCode_get(r);
	}

	def(orientHome) {
		setCharacter();
		auto& map = game.getMap();
		auto info = c->getHomeBuilding(map);
		bool r = c->orientBuilding(game, info);
		return ActionCode_get(r);
	}

	def(locateWork) {
		setCharacter();
		if (!c->getJob())
			return ActionCode::FAILURE; // no job

		auto info = c->getWorkBuilding(game);
		bool r = c->locateBuilding(game.getMap(), info);
		return ActionCode_get(r);
	}

	def(locateHome) {
		setCharacter();
		auto& map = game.getMap();
		auto info = c->getHomeBuilding(map);
		bool r = c->locateBuilding(map, info);
		return ActionCode_get(r);
	}

	def(enter) {
		setCharacter();
		bool r = c->makeInside(game);
		return ActionCode_get(r);
	}
	
	def(leave) {
		setCharacter();

		c->makeOutside(game);		
		return ActionCode::SUCCESS;
	}


	def(enterWork) {
		setCharacter();
		if (!c->job)
			return ActionCode::FAILURE;

		c->job->onEnter(c, game.getCalendar());
		return ActionCode::SUCCESS;
	}

	def(leaveWork) {
		setCharacter();
		Job* job = c->getJob();

		if (!job)
			return ActionCode::FAILURE;

		job->onLeave(c, game.getCalendar());
		return ActionCode::SUCCESS;
	}

	def(passWork) {
		setCharacter();
		Job* job = c->getJob();
		if (!job) {
			// No job so no need to work
			return ActionCode::FAILURE;
		}

		auto& calendar = game.getCalendar();
		auto hour = job->getNextLeaveHour(c, calendar);

		if (calendar.indicator > hour) 
			return ActionCode::SUCCESS;

		job->work(c, game);
		return ActionCode::PENDING;
	}


	
	
};


give(result, &runDayJob);

give(runDayJob, &workSection, &chillSection);

give(workSection, &mustWork, &workDay);

give(workDay,
	&leave,
	&orientCar,
	&walk,
	&locateWork,
	&drive,
	&orientWork,
	&walk,
	&enter,
	&enterWork,
	&passWork,
	&leaveWork,
	&leave,
	&orientCar,
	&walk,
	&locateHome,
	&drive,
	&orientHome,
	&walk,
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
