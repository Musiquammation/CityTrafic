#include "Game.hpp"

#include "Car.hpp"
#include "Character.hpp"
#include "Cell.hpp"
#include "Map.hpp"

#include <stdio.h>


void Game::test() {
	
}

void Game::frame() {
	this->test();


	// Car logic
	this->carHandler.updateCars(this);
	this->carHandler.moveCars();


	// Character logic
	for (Character* character : this->characterHandler) {
		character->frame(*this);
	}

	// Update grid
	this->map.resetCarMarks();
	for (auto& [pos, car] : this->carHandler) {
		auto cell = this->map.getEditCell(car->x, car->y);
		cell->setCarOn();
	}


	this->calendar.move();
	this->frameCount++;
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

BuildingInfo Game::getBuilding(int x, int y) {
	return this->map.getBuilding(x, y);
}




int Game::getFrame() const {
	return this->frameCount;
}


bool Game::checkBounds(int x, int y, int width, int height) const {
	return this->map.checkBounds(x,y,width,height);
}
