#include "Game.hpp"

#include "Car.hpp"
#include "Cell.hpp"

#include <stdio.h>

void Game::frame() {

	this->carHandler.updateCars();

	this->carHandler.moveCars();

	this->map.resetCarMarks();

	for (auto& [pos, car] : this->carHandler) {
		Cell* cell = this->map.getCell(car->x, car->y);
		cell->setCarOn();
		printf("%d %d\n", car->x, car->y);
	}
}

