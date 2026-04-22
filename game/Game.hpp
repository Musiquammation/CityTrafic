#pragma once

#include "Map.hpp"
#include "CarHandler.hpp"
#include "CharacterHandler.hpp"

#include "declarations.hpp"

class Game {
private:
    Map map{32,32};
    CarHandler carHandler{};
    CharacterHandler characterHandler{};
    int frameCount = 0;
    void test();

public:
    friend class Api;
    friend class Server;
    friend int main();


    void frame();

    Cell* getEditCell(int x, int y);
    const Cell* getCell(int x, int y);
    Car* spawnCar(int x, int y, Direction direction);
    Car* getCar(int x, int y);

    int getFrame() const;
    bool checkBounds(int x, int y, int width, int height) const;

};