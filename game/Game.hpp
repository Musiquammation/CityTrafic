#pragma once

#include "Map.hpp"
#include "CarHandler.hpp"
#include "declarations.hpp"

class Game {
private:
    Map map{32,32};
    CarHandler carHandler{};

public:
    friend struct Api;
    friend int main();


    void frame();

    Cell* getEditCell(int x, int y);
    const Cell* getCell(int x, int y);
    Car* spawnCar(int x, int y, Direction direction);
    Car* getCar(int x, int y);


};