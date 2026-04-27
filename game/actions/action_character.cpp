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
		setCharacter();


		if (c->state == CharacterState::INSIDE) {
			int x = (int)c->x;
			int y = (int)c->y;
			auto info = game.getMap().getBuilding(x, y);
			if (!info.building) {
				throw std::runtime_error{"Missing building"};
			}

			Vector<int> entry;
			Vector<int> exit;
			// Select entry and exit
			{
				Vector<int> list[info.building->getBufferLargeLength()];
				int entryLength = info.building->fillEntryList(list);
				entry = list[c->takeRandomPointId(entryLength)];
				
				int exitLength = info.building->fillExitList(list);
				exit = list[c->takeRandomPointId(exitLength)];
			}

			c->x = (float)entry.x + .5f;
			c->y = (float)entry.y + .5f;

			// Define path and go outside
			c->makeWalk(game, exit.x, exit.y);
			return ActionCode::PENDING;
		}
		
		
		if (c->state == CharacterState::WALK) {
			char dir = c->data.walk.path[c->data.walk.position];

			switch (dir) {
			case 0: // right
			{
				float px = c->x, nx = px + Character::FRONT_SPEED;
				if (frac(px) < .5f && frac(nx) >= .5f)
					c->data.walk.position++;
				c->x = nx;
				break;
			}

			case 1: // top-right
			{
				float px = c->x, nx = px + Character::SIDE_SPEED;
				float py = c->y, ny = py - Character::SIDE_SPEED;
				if ((frac(px) < .5f && frac(nx) >= .5f) || (frac(py) > .5f && frac(ny) <= .5f))
					c->data.walk.position++;
				c->x = nx; c->y = ny;
				break;
			}

			case 2: // up
			{
				float py = c->y, ny = py - Character::FRONT_SPEED;
				if (frac(py) > .5f && frac(ny) <= .5f)
					c->data.walk.position++;
				c->y = ny;
				break;
			}

			case 3: // top-left
			{
				float px = c->x, nx = px - Character::SIDE_SPEED;
				float py = c->y, ny = py - Character::SIDE_SPEED;
				if ((frac(px) > .5f && frac(nx) <= .5f) || (frac(py) > .5f && frac(ny) <= .5f))
					c->data.walk.position++;
				c->x = nx; c->y = ny;
				break;
			}

			case 4: // left
			{
				float px = c->x, nx = px - Character::FRONT_SPEED;
				if (frac(px) > .5f && frac(nx) <= .5f)
					c->data.walk.position++;
				c->x = nx;
				break;
			}

			case 5: // bottom-left
			{
				float px = c->x, nx = px - Character::SIDE_SPEED;
				float py = c->y, ny = py + Character::SIDE_SPEED;
				if ((frac(px) > .5f && frac(nx) <= .5f) || (frac(py) < .5f && frac(ny) >= .5f))
					c->data.walk.position++;
				c->x = nx; c->y = ny;
				break;
			}

			case 6: // down
			{
				float py = c->y, ny = py + Character::FRONT_SPEED;
				if (frac(py) < .5f && frac(ny) >= .5f)
					c->data.walk.position++;
				c->y = ny;
				break;
			}

			case 7: // bottom-right
			{
				float px = c->x, nx = px + Character::SIDE_SPEED;
				float py = c->y, ny = py + Character::SIDE_SPEED;
				if ((frac(px) < .5f && frac(nx) >= .5f) || (frac(py) < .5f && frac(ny) >= .5f))
					c->data.walk.position++;
				c->x = nx; c->y = ny;
				break;
			}

			default:
				printf("Finished\n");
				break;
		}

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
