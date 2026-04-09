#include "api.hpp"
#include "Game.hpp"
#include "Cell.hpp"
#include "Car.hpp"

#include <array>
#include <stdio.h>
#include <iostream>
#include <stdexcept>

#include "debugFile.hpp"


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



int main() {
	Api* api = Api_create();

	int crossX = 19;
	int crossY = 5;
	for (int i = 0; i < 31; i++)
		api->game.getEditCell(i, crossY)->setType(CellType::ROAD);

	for (int i = 0; i < 31; i++)
		api->game.getEditCell(crossX, i)->setType(CellType::ROAD);


	Car* cars[] = {
		// api->game.spawnCar(0, crossY, Direction::RIGHT),
		// api->game.spawnCar(1, crossY, Direction::RIGHT),
		// api->game.spawnCar(2, crossY, Direction::RIGHT),

		api->game.spawnCar(crossX, 5, Direction::UP),
		api->game.spawnCar(crossX, 6, Direction::UP),
		
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

		MapSize size = api->game.map.getMapSize();
		for (int y = size.y; y < size.height; y++) {
			for (int x = size.x; x < size.width; x++) {
				Cell* cell = api->game.map.getEditCell(x, y);
				if (cell->hasCar()) {
					Car* car = api->game.getCar(x, y);
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
			Api_frame(api);
		} catch (const std::exception& error) {
			rcerr << "Error: " << error.what() << std::endl;
			goto stopLoop;
		}

		fprintf(debugFile, "FRAME:END(%d)\n", frame);
		
	}

	stopLoop:
	fclose(debugFile);

	Api_delete(api);

	printf("Success!\n");
	return 0;
}