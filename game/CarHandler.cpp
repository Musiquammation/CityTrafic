#include "CarHandler.hpp"

#include "Car.hpp"

Car* CarHandler::spawnCar(int x, int y, Direction direction) {
	auto car = new Car{x, y, direction};

	this->cars[{x,y}] = car;
	return car;
}

void CarHandler::updateCars(Game* game) {
	for (auto& pair : this->cars) {
		pair.second->update(game);
	}
}

void CarHandler::moveCars() {
	std::unordered_map<std::pair<int,int>, Car*, pair_hash> newCars;

	for (auto& pair : this->cars) {
		Car* car = pair.second;
		car->move();
		newCars[{car->x, car->y}] = car;
	}

	this->cars = std::move(newCars);
}

Car* CarHandler::getCar(int x, int y) {
	return this->cars[{x,y}];
}



CarHandler::~CarHandler() {
	for (auto& pair : this->cars) {
		delete pair.second;
	}

	this->cars.clear();
}