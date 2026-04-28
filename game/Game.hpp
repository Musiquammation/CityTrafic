#pragma once

#include "Map.hpp"
#include "CarHandler.hpp"
#include "CharacterHandler.hpp"
#include "BuildingInfo.hpp"

#include "declarations.hpp"

class Game {
private:
    Map map{32,32};
    CarHandler carHandler{};
    CharacterHandler characterHandler{};
    int frameCount = 0;
    void test();

public:
    friend struct GameCommand;
    friend class Api;
    friend class Server;
    friend uint32_t* entities_helper_make(
        Game& game,
        int x, int y, int w, int h,
        uint8_t prefix
    );
    friend void entities_helper_read(Game& game, void* args);

    friend int main();


    void frame();

    Cell* getEditCell(int x, int y);
    const Cell* getCell(int x, int y);
    Car* spawnCar(int x, int y, Direction direction);
    Car* getCar(int x, int y);
    BuildingInfo getBuilding(int x, int y);
    const Map& getMap() const {return this->map;}
    Map& getMap() {return this->map;}

    int getFrame() const;
    bool checkBounds(int x, int y, int width, int height) const;

};