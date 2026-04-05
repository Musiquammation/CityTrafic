#include "CarHandler.hpp"

#include "Car.hpp"

Car* CarHandler::spawnCar(int x, int y, Direction direction) {
    auto car = new Car{x, y, direction};

    this->cars[{x,y}] = car;
    return car;
}

void CarHandler::updateCarPositions() {
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