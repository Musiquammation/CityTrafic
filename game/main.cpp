#include "api.hpp"
#include "Game.hpp"
#include "Cell.hpp"

#include <array>
#include <stdio.h>

int main() {
	Api* api = Api_create();

	for (int i = 1; i < 30; i++)
		api->game.getCell(i, 5)->setType(CellType::ROAD);


	api->game.spawnCar(1, 5, Direction::RIGHT);


	FILE* file = fopen("draft/output.txt", "w");
	if (!file) {
		printf("Cannot open output file\n");
		return 1;
	}

	for (int frame = 0; frame < 240; frame++) {
		fprintf(file, "FRAME:START(%d)\n", frame);

		MapSize size = api->game.map.getMapSize();
		for (int y = size.y; y < size.height; y++) {
			for (int x = size.x; x < size.width; x++) {
				Cell* cell = api->game.map.getCell(x, y);
				if (cell->hasCar()) {
					fprintf(file, "[%02d]", (int)cell->getType());
				} else if (cell->getType() == CellType::NONE) {
					fprintf(file, " .. ");
				} else {
					fprintf(file, " %02d ", (int)cell->getType());
				}
			}

			fprintf(file, "\n");
		}

		Api_frame(api);

		fprintf(file, "FRAME:END(%d)\n", frame);
	}

	fclose(file);

	Api_delete(api);

	printf("Success!\n");
	return 0;
}