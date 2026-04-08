#include "Game.hpp"

#include "Car.hpp"
#include "Cell.hpp"

#include <stdio.h>

void Game::frame() {
	this->carHandler.updateCars(this);

	this->carHandler.moveCars();

	this->map.resetCarMarks();

	printf("FRAME: ");
	for (auto& [pos, car] : this->carHandler) {
		auto cell = this->map.getEditCell(car->x, car->y);
		cell->setCarOn();
	}

	printf("\n");
}

Cell* Game::getEditCell(int x, int y) {
	return this->map.getEditCell(x, y);
}

const Cell* Game::getCell(int x, int y) {
	return this->map.getCell(x, y);
}

Car* Game::spawnCar(int x, int y, Direction direction) {
	auto cell = this->map.getEditCell(x, y);
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