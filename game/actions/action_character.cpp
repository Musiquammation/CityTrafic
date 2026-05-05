#include "action_character.hpp"

#include "ActionNode.hpp"
#include "actionTemplates.hpp"

#include "../Game.hpp"
#include "../Map.hpp"
#include "../Car.hpp"
#include "../Job.hpp"
#include "../JobOffer.hpp"
#include "../Building.hpp"
#include "../Character.hpp"

#include "../jobs/OilFieldJob.hpp"
#include "../jobs/CashierJob.hpp"

#include "../DebugLogger.hpp"

static DebugLogger printStatus{"Status", true};
static DebugLogger print{"Action", true};
static DebugLogger printWalk{"Walk", false};
static DebugLogger printDrive{"Drive", false};


#include <cmath>

startns(character)

static inline float frac(float x) {
	return x - std::floor(x);
}

#define LIST(all, fst, run, link)\
	fst(result)\
		all(work)\
			fst(enshureWork)\
		all(outWorkActivities)\
			fst(enshureLargeCarFuel)\
			fst(enshureFood)\
				all(enshureFood_sub)\
			fst(restAtHome)\
	fst(enshureCarFuel)\
	all(collectFuel)\
	all(walkToCar)\
	all(goHome)\
	\
	run(drive)\
	run(walk)\
	run(mustWork)\
	run(orientCar)\
	run(orientWork)\
	run(orientHome)\
	run(locateWork)\
	run(locateHome)\
	run(enter)\
	run(leave)\
	run(enterWork)\
	run(leaveWork)\
	run(passWork)\
	run(checkHomeOwnership)\
	run(checkFuelLarge)\
	run(isAtHome)\
	run(locateFuelStation)\
	run(orientFuelStation)\
	run(fillCarFuel)\
	run(checkFuel)\
	run(hasWork)\
	run(searchWork)\
	run(checkFood)\
	run(locateGrocery)\
	run(orientGrocery)\
	run(buySeeds)\
	run(waitSeedsConsumption)\
			
		



declList();



// Override setData
#undef setData
#define setCharacter() Character* c = (Character*)_data;

struct CharacterFriend {
	static ActionCode locate(Game& game, Character* character, BuildingType type) {
		auto p = character->getPos();
		auto& map = game.getMap();
		auto info = map.searchBuilding(
			p.x, p.y, type);

		if (!info.building)
			return ActionCode::FAILURE;

		return ActionCode_get(character->locateBuilding(map, info));
	}

	static ActionCode orient(Game& game, Character* character, BuildingType type) {
		auto p = character->getPos();
		auto info = game.getMap().searchBuilding(
			p.x, p.y, BuildingType::OIL_FIELD);

		if (!info.building)
			return ActionCode::FAILURE;

		return ActionCode_get(character->orientBuilding(game, info));

	}


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
			if (salary>0) {
				c->salaryEstimation = salary;
			}
			
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

	def(drive) {
		printDrive("\n");
		setCharacter();
		if (c->data.drive.state == ActionCode::PENDING) {
			return ActionCode::PENDING;
		}

		c->setState(CharacterState::OUTSIDE);
		return c->data.drive.state;
	}

	def(walk) {
		printWalk("\n");
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
		calendar_t m = c->nextRentPayMonth;
		printStatus("  next=%d now=%d\n", m, calendar.totalMonth);
		if ((m > calendar.totalMonth || (
			m == calendar.totalMonth && calendar.day < 9
		))) {
			return ActionCode::SUCCESS;
		}

		if (building->type != BuildingType::HOME) {
			throw std::runtime_error{
				"Home must be of type BuildingType::HOME"};
		}

		printStatus("  pay rent money=%d rent=%d\n",
			c->money, building->home.rent);

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

	def(isAtHome) {
		setCharacter();
		printStatus("isAtHome\n");
		printStatus("  money %d\n", c->money);

		if (!c->isInside())
			return ActionCode::FAILURE;

		auto p = c->getPos();
		auto info = game.getBuilding(p.x, p.y);
		if (info.x == c->home.x && info.y == c->home.y) {
			printStatus("  success\n");
			return ActionCode::SUCCESS;
		}

		printStatus("  failure\n");
		return ActionCode::FAILURE;	
	}

	def(locateFuelStation) {
		setCharacter();
		printStatus("locateFuelStation\n");
		return locate(game, c, BuildingType::OIL_FIELD);
	}

	def(orientFuelStation) {
		setCharacter();
		printStatus("orientFuelStation\n");
		return orient(game, c, BuildingType::OIL_FIELD);
	}

	def(fillCarFuel) {
		setCharacter();
		printStatus("fillCarFuel\n");

		if (!c->isInside() || !c->car)
			return ActionCode::FAILURE;

		Building* building = c->getCurrentBuilding(
			game.getMap(), BuildingType::OIL_FIELD).building;
		

		Job* _job = game.getJob(building->oilField.jobIdx);
		OilFieldJob& job = dynamic_cast<OilFieldJob&>(*_job);
		float price = job.getPricePerLiter(game);
		float completion = c->car->getFuel();
		float cost = Character::evalFullLiterSafetyCost(completion);
		if ((cost * price) > (float)c->money) {
			return ActionCode::SUCCESS;
		}

		float fuel = job.buy(game, 1);
		printStatus(
			"  price=%f money=%d completion=%f cost=%f fuel=%f\n",
			price,
			c->money,
			completion,
			cost,
			fuel
		);
		if (fuel <= 0) {
			if (completion < 5.0f)
				return ActionCode::FAILURE;

			return ActionCode::SUCCESS;
		}
		
		bool r = c->car->appendFuel(fuel);
		c->money--;
		return r ? ActionCode::SUCCESS : ActionCode::PENDING;
	}

	def(checkFuel) {
		setCharacter();
		printStatus("checkFuel\n");

		return ActionCode_get(c->car->getFuel() >= 1.0f);
	}

	def(checkFuelLarge) {
		setCharacter();
		printStatus("checkFuelLarge\n");

		return ActionCode_get(c->car->getFuel() >= 10.0f);
	}

	def(hasWork) {
		setCharacter();
		printStatus("hasWork\n");

		return ActionCode_get(c->job);
	}

	def(searchWork) {
		setCharacter();
		printStatus("searchWork\n");

		JobOffer offer;
		Job* job = game.searchJob(c, offer);
		
		printStatus("  got %p %d\n", job, (int)offer.type);
		if (!job)
			return ActionCode::FAILURE;

		return ActionCode_get(c->takeJob(job,
			offer, game.getCalendar()));
	}


	def(checkFood) {
		setCharacter();
		printStatus("checkFood\n");
		printStatus("  seeds=%.f\n", c->seeds);

		return ActionCode_get(c->seeds > Character::CHECK_SEEDS);
	}

	def(locateGrocery) {
		setCharacter();
		printStatus("locateGrocery\n");
		return locate(game, c, BuildingType::GROCERY);
	}

	def(orientGrocery) {
		setCharacter();
		printStatus("orientGrocery\n");
		return orient(game, c, BuildingType::GROCERY);
	}

	def(buySeeds) {
		setCharacter();
		printStatus("buySeeds\n");

		auto p = character->getPos();
		auto& map = game.getMap();
		auto info = map.searchBuilding(
			p.x, p.y, BuildingType::GROCERY);

		if (!info.building)
			return ActionCode::FAILURE;

		
		auto& job = dynamic_cast<CashierJob&>(*game.getJob(
			info.building->grocery.jobIdx));

		float diff = (float)Character::MAX_SEEDS - c->seeds;
		float maxAffordableSeeds = floorf((float)c->money / job.seedPrice);

		float seedsBought;

		if (maxAffordableSeeds >= diff) {
			seedsBought = diff;
		} else {
			seedsBought = maxAffordableSeeds;
		}

		c->seeds += seedsBought;
		c->money -= (int)ceilf(seedsBought * job.seedPrice);


		static constexpr float DELAY_PER_SEED = 1.1f;
		c->data.inside.grocery.delay = (int)ceilf(seedsBought * DELAY_PER_SEED);
		return ActionCode::SUCCESS;
	}

	def(waitSeedsConsumption) {
		setCharacter();
		printStatus("waitSeedsConsumption\n");

		c->data.inside.grocery.delay--;
		if (c->data.inside.grocery.delay <= 0)
			return ActionCode::SUCCESS;

		return ActionCode::PENDING;
	}





	
	
};


graph(result,
	&work,
	&outWorkActivities
);

graph(outWorkActivities,
	&enshureLargeCarFuel,
	&enshureFood,
	&restAtHome
);

graph(enshureFood,
	&checkFood,
	&enshureFood_sub
);

graph(enshureFood_sub,
	&leave,
	&walkToCar,
	&locateGrocery,
	&drive,
	&leave,
	&orientGrocery,
	&walk,
	&enter,
	&buySeeds,
	&waitSeedsConsumption,
	&leave
);

graph(enshureWork,
	&hasWork,
	&searchWork
);

graph(work,
	&enshureWork,
	&mustWork,
	&leave,
	&enshureCarFuel,
	&walkToCar,
	&locateWork,
	&drive,
	&orientWork,
	&walk,
	&enter,
	&enterWork,
	&passWork,
	&leaveWork,
	&leave
);

graph(enshureLargeCarFuel,
	&checkFuelLarge,
	&collectFuel
);


graph(enshureCarFuel,
	&checkFuel,
	&collectFuel
);

graph(collectFuel,
	&walkToCar,
	&locateFuelStation,
	&drive,
	&leave,
	&orientFuelStation,
	&walk,
	&enter,
	&fillCarFuel,
	&leave
);

graph(walkToCar,
	&leave,
	&orientCar,
	&walk
);

graph(restAtHome,
	&isAtHome,
	&goHome
);

graph(goHome,
	&enshureCarFuel,
	&checkHomeOwnership,
	&walkToCar,
	&locateHome,
	&drive,
	&orientHome,
	&walk,
	&enter
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

finishns()
