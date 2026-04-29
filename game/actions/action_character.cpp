#include "action_character.hpp"

#include "ActionNode.hpp"
#include "actionTemplates.hpp"

#include "../Game.hpp"
#include "../Map.hpp"
#include "../Car.hpp"
#include "../Job.hpp"
#include "../Building.hpp"
#include "../Character.hpp"

#include "../DebugLogger.hpp"

static DebugLogger printStatus{"Status", false};
static DebugLogger print{"Action", false};


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
	run(checkHomeOwnership);\
			
		



declList();



// Override setData
#undef setData
#define setCharacter() Character* c = (Character*)_data;

struct CharacterFriend {
	def(mustWork) {
		printStatus("mustWork\n");
		setCharacter();
		Job* job = c->getJob();
		if (!job) {
			// No job so no need to work
			return ActionCode::FAILURE;
		}

		auto& calendar = game.getCalendar();
		auto hour = job->getNextEnterHour(c, calendar);

		// No need to wake up
		if (calendar.indicator < hour)
			return ActionCode::FAILURE;
		
		
		// Day of the pay
		if (calendar.day == 0) {
			int waiting = c->waitingJoyMoney;
			if (waiting > 0) {
				int earn = job->pay(waiting);
				c->money += earn;
				waiting -= earn;
				if (waiting > 0)
					return ActionCode::FAILURE;
			}

			int salary = job->getSalary(c, calendar);
			int earned = job->pay(salary);
			printStatus("earn %d | %d\n", earned, salary);
			c->money += earned;
			salary -= earned;
			if (salary > 0) {
				c->waitingJoyMoney = waiting + salary;
				return ActionCode::FAILURE;
			}
		}

		return ActionCode::SUCCESS;
	}
	
	def(isChillDay) {
		printStatus("isChillDay\n");
		return ActionCode::FAILURE;
	}

	def(chillDayTest) {
		printStatus("chillDayTest\n");
		return ActionCode::PENDING;
	}

	def(drive) {
		printStatus("drive\n");
		setCharacter();
		if (c->data.drive.state == ActionCode::PENDING) {
			return ActionCode::PENDING;
		}

		c->setState(CharacterState::OUTSIDE);
		return c->data.drive.state;
	}

	def(walk) {
		printStatus("walk\n");
		setCharacter();
		return c->walk(game);
	}

	def(orientCar) {
		printStatus("orientCar\n");
		setCharacter();
		if (!c->car)
			return ActionCode::FAILURE;

		
		bool r = c->makeWalk(game, c->car->x, c->car->y);
		printStatus("walk %d => %d %d\n", r, c->car->x, c->car->y);
		return ActionCode_get(r);
	}

	def(orientWork) {
		printStatus("orientWork\n");
		setCharacter();
		if (!c->getJob())
			return ActionCode::FAILURE; // no job
			
		auto info = c->getWorkBuilding(game);
		bool r = c->orientBuilding(game, info);
		return ActionCode_get(r);
	}

	def(orientHome) {
		printStatus("orientHome\n");
		setCharacter();
		auto& map = game.getMap();
		auto info = c->getHomeBuilding(map);
		bool r = c->orientBuilding(game, info);
		return ActionCode_get(r);
	}

	def(locateWork) {
		printStatus("locateWork\n");
		setCharacter();
		if (!c->getJob())
			return ActionCode::FAILURE; // no job

		auto info = c->getWorkBuilding(game);
		bool r = c->locateBuilding(game.getMap(), info);
		return ActionCode_get(r);
	}

	def(locateHome) {
		printStatus("locateHome\n");
		setCharacter();
		auto& map = game.getMap();
		auto info = c->getHomeBuilding(map);
		bool r = c->locateBuilding(map, info);
		return ActionCode_get(r);
	}

	def(enter) {
		printStatus("enter\n");
		setCharacter();
		bool r = c->makeInside(game);
		printStatus("  result=%d\n", r);
		return ActionCode_get(r);
	}
	
	def(leave) {
		printStatus("leave\n");
		setCharacter();

		c->makeOutside(game);		
		return ActionCode::SUCCESS;
	}


	def(enterWork) {
		printStatus("enterWork\n");
		setCharacter();
		if (!c->job)
			return ActionCode::FAILURE;

		c->job->onEnter(c, game.getCalendar());
		return ActionCode::SUCCESS;
	}

	def(leaveWork) {
		printStatus("leaveWork\n");
		setCharacter();
		Job* job = c->getJob();

		if (!job)
			return ActionCode::FAILURE;

		job->onLeave(c, game.getCalendar());
		return ActionCode::SUCCESS;
	}

	def(passWork) {
		printStatus("passWork\n");
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

	def(checkHomeOwnership) {
		printStatus("checkHomeOwnership\n");
		setCharacter();

		if (c->state == CharacterState::INSIDE) {
			throw std::runtime_error{
				"Characters cannot check home "
				"while being inside a building"
			};
		}


		
		// No home
		if (c->home.x == INT32_MIN) {
			return ActionCode::FAILURE;
		}

		auto& calendar = game.getCalendar();
		Building* building = c->getHomeBuilding(
			game.getMap()).building;

		// Check rent date
		int m = c->nextRentPayMonth;
		if ((m < calendar.totalMonth || (
			m == calendar.totalMonth && calendar.day >= 9
		))) {
			return ActionCode::SUCCESS;
		}

		if (building->type != BuildingType::HOME) {
			throw std::runtime_error{
				"Home must be of type BuildingType::HOME"};
		}

		int rent = building->home.rent;
		// Pay rent
		if (c->money >= rent) {
			c->pay(rent);
			Player* owner = game.getPlayer(building->owner);
			if (owner) {
				owner->money += rent;
			}

			// Mark rent as paid
			c->nextRentPayMonth = calendar.totalMonth+1;
			return ActionCode::SUCCESS;
		}
		
		// Cannot pay rent, so character quits home
		c->home.x = INT32_MIN; // no home
		return ActionCode::FAILURE;
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
