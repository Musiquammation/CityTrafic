#include <iostream>

#ifndef COMPILE_SERVER

#define MAIN_TEST_ID 2



struct RedCerr {
	template<typename T>
	RedCerr& operator<<(const T& value) {
		std::cerr << "\033[31m" << value << "\033[0m";
		return *this;
	}

	// Pour std::endl et manipulators
	RedCerr& operator<<(std::ostream& (*manip)(std::ostream&)) {
		std::cerr << manip;
		return *this;
	}
};

RedCerr rcerr;



#if MAIN_TEST_ID == 0

#include "Game.hpp"
#include "Cell.hpp"
#include "Car.hpp"

#include <array>
#include <stdio.h>
#include <stdexcept>

#include "debugFile.hpp"

int main() {
	Game game{};

	int crossX = 19;
	int crossY = 6;
	for (int i = 0; i < 31; i++)
		game.getEditCell(i, crossY)->setType(CellType::ROAD, game);

	for (int i = 0; i < 31; i++)
		game.getEditCell(crossX, i)->setType(CellType::ROAD, game);


	Car* cars[] = {
		game.spawnCar(1, crossY, Direction::RIGHT),


		game.spawnCar(crossX, 20, Direction::UP),
		game.spawnCar(crossX, 21, Direction::UP),
		game.spawnCar(crossX, 22, Direction::UP),
		game.spawnCar(crossX, 23, Direction::UP),
		game.spawnCar(crossX, 24, Direction::UP),
		game.spawnCar(crossX, 25, Direction::UP),
		game.spawnCar(crossX, 26, Direction::UP),
		game.spawnCar(crossX, 27, Direction::UP)
	};

	for (int i = 0; i < (int)(sizeof(cars) / sizeof(cars[0])); i++) {
		printf("car %d: %p\n", i, cars[i]);
	}
	


	debugFile = fopen("draft/output.txt", "w");
	if (!debugFile) {
		printf("Cannot open output debugFile\n");
		return 1;
	}

	for (int frame = 0; frame < 300; frame++) {
		fprintf(debugFile, "FRAME:START(%d)\n", frame);

		MapSize size = game.map.getMapSize();
		for (int y = size.y; y < size.height; y++) {
			for (int x = size.x; x < size.width; x++) {
				Cell* cell = game.map.getEditCell(x, y);
				if (cell->hasCar()) {
					Car* car = game.getCar(x, y);
					fprintf(debugFile, "%02d:%c:%.2f ", (int)cell->getType(), 'a' + (char)car->direction, car->step);
				} else if (cell->getType() == CellType::NONE) {
					fprintf(debugFile, "X ");
				} else {
					fprintf(debugFile, "%02d ", (int)cell->getType());
				}
			}

			fprintf(debugFile, "\n");
		}

		fprintf(debugFile, "FRAME:LOGS(%d)\n", frame);
		
		for (int i  = 0; i < (int)(sizeof(cars) / sizeof(cars[0])); i++) {
			Car* car = cars[i];
			debugLog("data [%d] y: %.2f, v: %.2f, a: %.3f\n", i,
			(float)car->y + car->step, car->getSpeed(), car->getAcceleration());
		}

		try {
			game.frame();
		} catch (const std::exception& error) {
			rcerr << "Error: " << error.what() << std::endl;
			goto stopLoop;
		}

		fprintf(debugFile, "FRAME:END(%d)\n", frame);
		
	}

	stopLoop:
	fclose(debugFile);


	printf("Success!\n");
	return 0;
}



#elif MAIN_TEST_ID == 1


#include "ActionExecutor.hpp"
#include "actions/action_test.hpp"
#include "Game.hpp"
#include "Character.hpp"

int main() {
	Game game;
	Character character;

	actionNodes::Test test{10, 5};
	ActionExecutor executor{actionNodes::Test::init(), &test, nullptr};
	
	while (1) {
		printf("[[run]]\n");
		if (executor.run(game, &character))
			break;
	}

	
	printf("Success!\n");
	return 0;
	
}


#elif MAIN_TEST_ID == 2


#include "direction.hpp"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

// Assuming the functions direction::getSide and setSide are defined above...

void print_bits(uint16_t data) {
    for (int i = 15; i >= 0; i--) {
        printf("%d", (data >> i) & 1);
        if (i % 4 == 0) printf(" ");
    }
    printf("\n");
}

int main() {
    printf("Starting tests...\n");

    // --- TEST 1: Basic standard values ---
    uint16_t data = 0; 
    data = direction::setSide(data, 0, 1); // Right = 1
    data = direction::setSide(data, 1, 2); // Up = 2
    assert(direction::getSide(data, 0) == 1);
    assert(direction::getSide(data, 1) == 2);
    assert((data >> 12 & 1) == 0); // Special flag should be 0
    printf("Test 1 Passed: Standard mode working.\n");

    // --- TEST 2: Activating Special ---
    // Set "Up" (direction 1) to special
    data = direction::setSide(data, 1, 4); 
    assert(direction::getSide(data, 1) == 4);
    assert((data >> 12 & 1) == 1);     // Special flag ON
    assert(((data >> 13) & 0x03) == 1); // Selector should point to 1
    printf("Test 2 Passed: Special flag and selector auto-activation.\n");

    // --- TEST 3: Multiple Special Sides (Chaining) ---
    // Set "Down" (direction 3) to special as well
    data = direction::setSide(data, 3, 4); 
    assert(direction::getSide(data, 1) == 4);
    assert(direction::getSide(data, 3) == 4);
    assert(direction::getSide(data, 0) == 1); // Right remains 1
    
    /* Internal Logic Check:
       Selector is 1 (Up).
       Offset at direction 1 should be 2 (to reach direction 3: (1+2)%4 = 3).
       Offset at direction 3 should be 0 (end of chain).
    */
    int idx_at_1 = (data >> (4 + 2 * 1)) & 0x03;
    int idx_at_3 = (data >> (4 + 2 * 3)) & 0x03;
    assert(idx_at_1 == 2);
    assert(idx_at_3 == 0);
    printf("Test 3 Passed: Multi-special chaining (idx calculation).\n");

    // --- TEST 4: Removing Special ---
    // Set "Up" back to a normal value
    data = direction::setSide(data, 1, 0); 
    assert(direction::getSide(data, 1) == 0);
    assert(direction::getSide(data, 3) == 4); // Down should still be special
    assert(((data >> 13) & 0x03) == 3); // Selector must now point to 3
    printf("Test 4 Passed: Special removal and selector update.\n");

    // --- TEST 5: Total Deactivation ---
    data = direction::setSide(data, 3, 2); 
    assert((data >> 12 & 1) == 0); // Special flag should turn OFF
    assert(direction::getSide(data, 3) == 2);
    printf("Test 5 Passed: Special flag auto-deactivation.\n");

    printf("\nAll tests passed successfully!\n");
    return 0;
}

#endif
#endif