#include "updateNet_helper.hpp"

#include "Game.hpp"
#include "CarHandler.hpp"
#include "Car.hpp"
#include "Character.hpp"

#include <stdint.h>


#define push64(val) {*(uint64_t*)ptr = (val); ptr += 2;}
#define push(val) {*ptr++ = val;}

uint32_t* updateNet_helper_write(
	Game& game,
	int x, int y, int w, int h,
	uint8_t clientRequestId,
	int money
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


	uint32_t fullSize = sizeof(uint32_t) * (
		+ 1 // money
		+ 1 // buffer
		+ 2 // date
		+ carsCount*5 // cars
		+ charactersCount*2 + 3 // characters
	);

	uint32_t* const buffer = (uint32_t*)malloc(fullSize + sizeof(uint32_t));
	*(uint8_t*)buffer = clientRequestId;

	uint32_t* ptr = buffer + 1;
	
	push(fullSize);
	*((int32_t*)ptr++) = money;
	ptr++; // buffer


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

	// Send characters
	push(charactersCount);
	for (auto character: game.characterHandler) {
		auto state = character->getState();
		if ((state == CharacterState::WALK ||
			state == CharacterState::OUTSIDE)
			&& character->x >= fx0 && character->x < fx1
			&& character->y >= fy0 && character->y < fy1
		) {
			push(*(uint32_t*)&character->x);
			push(*(uint32_t*)&character->y);
		}
	}


	return buffer;
}

#undef push64




void updateNet_helper_read(Game& game, void* args) {
    uint32_t* ptr = (uint32_t*)args;

	// Skip money, buffer and calendar
	ptr += 4;

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

    uint32_t charactersCount = *ptr++;
    for (uint32_t i = 0; i < charactersCount; i++) {
        float x = *(float*)(ptr++);
        float y = *(float*)(ptr++);

        Character* character = Character::createClientCharacter(x, y);
        game.characterHandler.pushCharacter(character);
    }
}

