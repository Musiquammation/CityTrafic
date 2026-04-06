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


    Cell* getCell(int x, int y);
    void frame();



};