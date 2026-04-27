#include "action_character.hpp"

#include "ActionNode.hpp"
#include "actionTemplates.hpp"

#include "../Game.hpp"
#include "../Map.hpp"
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
					link(takeCarToWork);\
					link(waitCarNotification);\
					link(walkToWork);\
					link(takeCarToHome);\
					link(waitCarNotification);\
					link(walkToHome);\
			all(chillSection);\
				run(isChillDay);\
				all(chillDay);\
					run(chillDayTest);\
	run(takeCarToWork);\
	run(takeCarToHome);\
	run(walkToWork);\
	run(walkToHome);\
	run(waitCarNotification);\
			
		



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

	def(takeCarToWork) {
		return ActionCode::PENDING;
	}
	
	def(directlyWalkToWork) {
		setCharacter();


		if (c->state == CharacterState::INSIDE) {
			int x = (int)c->x;
			int y = (int)c->y;
			auto entryInfo = game.getMap().getBuilding(x, y);
			if (!entryInfo.building) {
				throw std::runtime_error{"Missing entry building"};
			}
			
			
			auto exitInfo = c->getWorkBuilding(game.getMap());
			if (!exitInfo.building) {
				throw std::runtime_error{"Missing exit building"};
			}

			Vector<int> entry;
			Vector<int> exit;
			// Select entry and exit
			{
				int entryLargeLength = entryInfo.building->getBufferLargeLength();
				int exitLargeLength = exitInfo.building->getBufferLargeLength();
				int largeLength = entryLargeLength > exitLargeLength ?
					entryLargeLength : exitLargeLength;

				Vector<int> list[largeLength];

				int entryLength = entryInfo.building->fillEntryList(list);
				entry = list[c->takeRandomPointId(entryLength)];
				
				int exitLength = exitInfo.building->fillExitList(list);
				exit = list[c->takeRandomPointId(exitLength)];
			}

			c->x = (float)(entryInfo.x + entry.x) + .5f;
			c->y = (float)(entryInfo.y + entry.y) + .5f;

			// Define path and go outside
			bool result = c->makeWalk(game, exitInfo.x + exit.x, exitInfo.y + exit.y);

			return result ? ActionCode::PENDING : ActionCode::FAILURE;
		}
		
		
		if (c->state == CharacterState::WALK) {
			char dir = c->data.walk.path[c->data.walk.position];


			static constexpr float SQRT2 = 1.41421356f;
			static constexpr float INV_SQRT2 = 0.70710678f;
			static const Vector<float> DELTAS[] = {
				{ 1.0f,  0.0f},              // right
				{ INV_SQRT2, -INV_SQRT2},    // up-right
				{ 0.0f, -1.0f},              // up
				{-INV_SQRT2, -INV_SQRT2},    // up-left
				{-1.0f,  0.0f},              // left
				{-INV_SQRT2,  INV_SQRT2},    // down-left
				{ 0.0f,  1.0f},              // down
				{ INV_SQRT2,  INV_SQRT2}     // down-right
			};

			static const Vector<int> DIRECTIONS[] = {
				{ 1,  0},   // right
				{ 1, -1},   // up-right
				{ 0, -1},   // up
				{-1, -1},   // up-left
				{-1,  0},   // left
				{-1,  1},   // down-left
				{ 0,  1},   // down
				{ 1,  1}    // down-right
			};


			auto move = [c, &dir, &game]() {
				c->data.walk.anchor.x += DIRECTIONS[dir].x;
				c->data.walk.anchor.y += DIRECTIONS[dir].y;
				c->data.walk.position++;

				dir = c->data.walk.path[c->data.walk.position];

				if (dir != 8) {
					return ActionCode::PENDING;
				}

				auto info = game.getMap().getBuilding(
					c->data.walk.anchor.x,
					c->data.walk.anchor.y
				);

				printf("final %p\n", info.building);

				return c->makeInside(game) ?
					ActionCode::SUCCESS : ActionCode::FAILURE;
			};

			// Move step
			c->data.walk.step += Character::SPEED;
			if (dir%2 == 0) {
				if (c->data.walk.step > 1) {
					c->data.walk.step -= 1;
					ActionCode c = move();
					if (c != ActionCode::PENDING) {return c;}
				}
			} else if (c->data.walk.step > SQRT2) {
				c->data.walk.step -= SQRT2;
				ActionCode c = move();
				if (c != ActionCode::PENDING) {return c;}
			}

			c->x = (float)c->data.walk.anchor.x + .5f + DELTAS[dir].x * c->data.walk.step;
			c->y = (float)c->data.walk.anchor.y + .5f + DELTAS[dir].y * c->data.walk.step;

			printf("(%.3f, %3f | %d)\n", c->x, c->y, c->data.walk.position);

			return ActionCode::PENDING;
		}
		
		return ActionCode::FAILURE;	
	}
	
	def(takeCarToHome) {
		printf("take car to home\n");
		return ActionCode::PENDING;
	}
	
	def(walkToWork) {
		printf("walkToWork\n");
		return ActionCode::PENDING;
	}
	
	def(walkToHome) {
		printf("walkToHome\n");
		return ActionCode::PENDING;
	}
	
	def(waitCarNotification) {
		printf("waitCarNotification\n");
		return ActionCode::PENDING;
	}
	
	def(chillDayTest) {
		printf("chillDayTest\n");
		return ActionCode::PENDING;
	
	}
};


give(result, &runDayJob);

give(runDayJob, &workSection, &chillSection);

give(workSection, &isWorkDay, &workDay);

give(workDay,
    &takeCarToWork,
    &waitCarNotification,
    &walkToWork,
    &takeCarToHome,
    &waitCarNotification,
    &walkToHome
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
