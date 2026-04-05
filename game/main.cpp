#include "api.hpp"
#include "Game.hpp"
#include "Cell.hpp"

#include <array>
#include <stdio.h>

int main() {
    Api* api = Api_create();

    
    Cell* cell = api->game.map.getCell(15, 12);
    cell->data = 42;


    api->game.map.expand(-10, -10, 343, 32);

    Car* car = api->game.carHandler.spawnCar(3, 2, Direction::RIGHT);
    Car* c2 = api->game.carHandler.getCar(3, 2);


    api->game.frame();

    Api_delete(api);


    printf("Success!\n");
    return 0;
}