#include "Game.hpp"

#include "Car.hpp"
#include "Cell.hpp"

#include <stdio.h>

void Game::frame() {
	this->carHandler.updateCars(this);

	this->carHandler.moveCars();

	this->map.resetCarMarks();

	for (auto& [pos, car] : this->carHandler) {
		Cell* cell = this->map.getCell(car->x, car->y);
		cell->setCarOn();
	}
}

Cell* Game::getCell(int x, int y) {
	return this->map.getCell(x, y);
}

Car* Game::spawnCar(int x, int y, Direction direction) {
	Cell* cell = this->map.getCell(x, y);
	if (!cell || cell->hasCar()) {
		return nullptr;
	}

	Car* car = this->carHandler.spawnCar(x, y, direction);
	cell->setCarOn();
	return car;
}

Car* Game::getCar(int x, int y) {
	return this->carHandler.getCar(x, y);
}