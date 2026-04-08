#include "api.hpp"
#include "Game.hpp"
#include "Cell.hpp"
#include "Car.hpp"

#include <array>
#include <stdio.h>

#include "debugFile.hpp"

int main() {
	Api* api = Api_create();

	for (int i = 0; i < 31; i++)
		api->game.getEditCell(i, 5)->setType(CellType::ROAD);

	for (int i = 0; i < 31; i++)
		api->game.getEditCell(10, i)->setType(CellType::ROAD);


	Car* cars[] = {
		api->game.spawnCar(0, 5, Direction::RIGHT),
		api->game.spawnCar(10, 13, Direction::UP),
	};

	printf("%p\n", cars[0]);
	


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
		
		Api_frame(api);
		fprintf(debugFile, "FRAME:END(%d)\n", frame);
		
	}

	fclose(debugFile);

	Api_delete(api);

	printf("Success!\n");
	return 0;
}