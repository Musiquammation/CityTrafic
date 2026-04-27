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


#include "Game.hpp"
#include "Building.hpp"
#include "Character.hpp"
#include "Map.hpp"
#include "Vector.hpp"

#include <map>


int main() {
	Game game;

	int value = 12;
	int* valuePtr = &value;
    std::map<Vector<int>, int*> map;

	map[Vector<int>{10,12}] = valuePtr;

	auto it = map.find(Vector<int>{10,12});
	if (it != map.end()) {
		printf("%p\n", it->second);
	} else {
		printf("%p\n", nullptr);
	}



	
	printf("Success!\n");
	return 0;
	
}


#endif
#endif