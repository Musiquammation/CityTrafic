#include "Game.hpp"

#include "Car.hpp"
#include "Cell.hpp"

#include <stdio.h>


void Game::test() {
	
}

void Game::frame() {
	this->test();

	// Car behavior
	{
		// needs to read map for getDanger
		auto lockPositions = this->mutexPool.lockWrite(MutexLabel::CARS_STRUCTURE);
		auto lockMap = this->mutexPool.lockRead(MutexLabel::MAP);
		this->carHandler.updateCars(this);
	}

	// Move cars
	{
		auto lockPositions = this->mutexPool.lockWrite(MutexLabel::CARS_POSITIONS);
		this->carHandler.moveCars();
	}
	
	// Update car positions
	{
		auto lockMap = this->mutexPool.lockWrite(MutexLabel::MAP);
		auto lockStructure = this->mutexPool.lockWrite(MutexLabel::CARS_STRUCTURE);
		
		this->map.resetCarMarks();

		for (auto& [pos, car] : this->carHandler) {
			auto cell = this->map.getEditCell(car->x, car->y);
			cell->setCarOn();
		}


	}
	


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


int Game::getFrame() const {
	return this->frameCount;
}