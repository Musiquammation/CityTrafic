#include "updateNet_helper.hpp"

#include <set>

#include "Game.hpp"
#include "CarHandler.hpp"
#include "Car.hpp"
#include "Character.hpp"
#include "Job.hpp"

#include <stdint.h>

#include "DebugLogger.hpp"

static DebugLogger print{"net_helper", true};


#define push64(val) {*(uint64_t*)ptr = (val); ptr += 2;}
#define push(val) {*ptr++ = (uint32_t) val;}

uint32_t* updateNet_helper_write(
	Game& game,
	int x, int y, int w, int h,
	uint8_t clientRequestId,
	int money,
	bool updateClientJobs,
	int cellsLayerId,
	int mapPrecision,
	int rx0, int ry0, int rx1, int ry1
) {
	float fx0 = (float)x;
	float fy0 = (float)y;
	float fx1 = (float)(x+w);
	float fy1 = (float)(y+h);


	uint32_t carsCount = 0;
	uint32_t charactersCount = 0;

	for (auto [_, car]: game.carHandler) {
		if (car->x >= x && car->x < x+w
			&& car->y >= y && car->y < y+h
		) {
			carsCount++;
		}
	}

	for (auto character: game.characterHandler) {
		auto state = character->getState();
		if ((state == CharacterState::WALK ||
			state == CharacterState::OUTSIDE)
			&& character->x >= fx0 && character->x < fx1
			&& character->y >= fy0 && character->y < fy1
		) {
			charactersCount++;
		}
	}

	int jobSize;
	bool alignJobs;
	if (updateClientJobs) {
		jobSize = 0;

		alignJobs = (jobSize % 2 == 1);
		if (alignJobs) {
			jobSize++; // 64 bits alignement
		}

	} else {
		jobSize = 0;
	}


	// Count missed regions
	struct Region {
		uint64_t key;
		int32_t rx;
		int32_t ry;
	};

	std::vector<Region> missedInView;
	auto& editedCells = *(game.map.getEditLayer(cellsLayerId));

	for (auto it = editedCells.begin(); it != editedCells.end(); ) {
		const auto& i = *it;

		if (i.x >= rx0 && i.y >= ry0 && i.x <= rx1 && i.y <= ry1) {
			uint64_t key = (uint64_t(i.x) << 32) | uint64_t(i.y);

			missedInView.push_back({key, i.x, i.y});
			it = editedCells.erase(it);
			continue; // because we erased
		}

		++it;
	}



	// 64bits alignment
	int alignCars = (carsCount % 2 == 1) ? 1:0;

	uint32_t fullSize = sizeof(uint32_t) * (
		+ 1 // full size
		+ 1 // money
		+ 1 // jobSize
		+ (uint32_t)jobSize
		+ 2 // date
		+ 1 // cars count
		+ carsCount*5 // cars
		+ (uint32_t)alignCars
		+ 1 // character count
		+ charactersCount*4 // characters
		+ 1 // map length
		+ (uint32_t)((int)missedInView.size() * (
			4+4+ // x, y
			mapPrecision*mapPrecision/2 // data
		)) // map
	);

	auto const buffer = (uint32_t*)malloc(
		fullSize +
		sizeof(uint32_t)
	);
	*(uint8_t*)buffer = clientRequestId;

	uint32_t* ptr = buffer + 1;
	

	push(fullSize);
	*((int32_t*)ptr++) = money;
	push(jobSize);


	// Send jobs
	if (updateClientJobs) {
		if (alignJobs) {
			ptr++; // align
		}
	}


	// Send date and money
	push64(game.calendar.indicator);
	


	// Send cars
	push(carsCount);

	for (auto [_, car]: game.carHandler) {
		if (car->x >= x && car->x < x+w
			&& car->y >= y && car->y < y+h
		) {
			float speed = car->getSpeed();
			uint32_t flag = (uint32_t)car->state |
				((uint32_t)car->direction << 8);

			push(car->x);
			push(car->y);
			push(*(uint32_t*)&car->step);
			push(*(uint32_t*)&speed);
			push(flag);
		}
	}


	if (alignCars) {
		ptr++;
	}

	// Send characters
	push(charactersCount);
	for (auto character: game.characterHandler) {
		auto state = character->getState();
		if ((state == CharacterState::WALK ||
			state == CharacterState::OUTSIDE)
			&& character->x >= fx0 && character->x < fx1
			&& character->y >= fy0 && character->y < fy1
		) {
			push64((uint64_t)character);
			push(*(uint32_t*)&character->x);
			push(*(uint32_t*)&character->y);
		}
	}


	// Send updates
	if (!missedInView.empty()) {print("send %ld\n", missedInView.size());}
	push(missedInView.size());
	for (auto r: missedInView) {
		push(r.rx);
		push(r.ry);
		print("cpy %d %d [%d]\n", r.rx, r.ry, mapPrecision);

		game.map.copyCells(
			(Cell*)ptr,
			r.rx * mapPrecision,
			r.ry * mapPrecision,
			mapPrecision,
			mapPrecision
		);

		// move
		ptr += mapPrecision*mapPrecision/2;
	}


	return buffer;
}

#undef push64




void updateNet_helper_read(Game& game, void* args) {
	uint32_t* ptr = (uint32_t*)args;

	ptr++; // skip money
	int jobSize = *ptr++;
	ptr += jobSize; // skip jobs
	ptr += 2; // skip calendar

	game.map.resetCarMarks();
	game.carHandler.clear();

	uint32_t carsCount = *ptr++;

	for (uint32_t i = 0; i < carsCount; i++) {
		int x = (int)(*ptr++);
		int y = (int)(*ptr++);
		float step = *(float*)(ptr++);
		float speed = *(float*)(ptr++);
		uint32_t flag = *ptr++;
		Direction direction = (Direction)(flag >> 8);
		CarState state = (CarState)(flag & 0xff);

		Car* car = game.spawnCar(x, y, direction);
		car->step = step;
		car->state = state;
		car->setSpeed(speed);
	}

	if (carsCount % 2) {ptr++;}

	uint32_t charactersCount = *ptr++;
	for (uint32_t i = 0; i < charactersCount; i++) {
		ptr += 2; // pointer	
		float x = *(float*)(ptr++);
		float y = *(float*)(ptr++);

		Character* character = Character::createClientCharacter(x, y);
		game.characterHandler.pushCharacter(character);
	}
}

