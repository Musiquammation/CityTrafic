#include <iostream>


#define MAIN_TEST_ID 1



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
	int crossY = 5;
	for (int i = 0; i < 31; i++)
		game.getEditCell(i, crossY)->setType(CellType::ROAD);

	for (int i = 0; i < 31; i++)
		game.getEditCell(crossX, i)->setType(CellType::ROAD);


	Car* cars[] = {
		// game.spawnCar(0, crossY, Direction::RIGHT),
		// game.spawnCar(1, crossY, Direction::RIGHT),
		// game.spawnCar(2, crossY, Direction::RIGHT),

		game.spawnCar(crossX, 23, Direction::UP),
		game.spawnCar(crossX, 24, Direction::UP),
		game.spawnCar(crossX, 25, Direction::UP),
		game.spawnCar(crossX, 26, Direction::UP),
		game.spawnCar(crossX, 27, Direction::UP),
		game.spawnCar(crossX, 28, Direction::UP),
		game.spawnCar(crossX, 29, Direction::UP),
		game.spawnCar(crossX, 30, Direction::UP),
		
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
	ActionNodeExecutor executor{actionNodes::Test::init(), &test, nullptr};
	
	while (1) {
		printf("[[run]]\n");
		if (executor.run(game, &character))
			break;
	}

	
	printf("Success!\n");
	return 0;
	
}


#endif