#include "entities_helper.hpp"

#include "Game.hpp"
#include "CarHandler.hpp"
#include "Car.hpp"
#include "Character.hpp"

#include <stdint.h>

/**
 * @warning This function use the prefix `prefix`
 */
uint32_t* entities_helper_make(
	Game& game,
	int x, int y, int w, int h,
	uint8_t prefix
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
		if (character->state == CharacterState::WALK
			&& character->x >= fx0 && character->x < fx1
			&& character->y >= fy0 && character->y < fy1
		) {
			charactersCount++;
		}
	}


	uint32_t fullSize = sizeof(uint32_t) *
		(carsCount*5 + charactersCount*2 + 3);

	uint32_t* const buffer = (uint32_t*)malloc(fullSize + sizeof(uint32_t));
	*(uint8_t*)buffer = prefix;

	uint32_t* ptr = buffer + 1;
	
	*ptr++ = fullSize;

	// Send cars
	*ptr++ = carsCount;

	for (auto [_, car]: game.carHandler) {
		if (car->x >= x && car->x < x+w
			&& car->y >= y && car->y < y+h
		) {
			float speed = car->getSpeed();
			uint32_t flag = (uint32_t)car->state |
				((uint32_t)car->direction << 8);

			*ptr++ = car->x;
			*ptr++ = car->y;
			*ptr++ = *(uint32_t*)&car->step;
			*ptr++ = *(uint32_t*)&speed;
			*ptr++ = flag;
		}
	}

	// Send characters
	*ptr++ = charactersCount;
	for (auto character: game.characterHandler) {
		if (character->state == CharacterState::WALK
			&& character->x >= fx0 && character->x < fx1
			&& character->y >= fy0 && character->y < fy1
		) {
			*ptr++ = *(uint32_t*)&character->x;
			*ptr++ = *(uint32_t*)&character->y;
		}
	}


	return buffer;
}


void entities_helper_read(Game& game, void* args) {
    uint32_t* ptr = (uint32_t*)args;

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

        Character* character = new Character{x, y, CharacterState::WALK};
        game.characterHandler.pushCharacter(character);
    }
}
