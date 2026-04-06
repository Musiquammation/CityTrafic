#pragma once

#include "declarations.hpp"
#include "Vector.hpp"
#include "Direction.hpp"
#include "PathHandler.hpp"

#include <stdint.h>

enum class CarState: uint8_t {
    FRONT,
    TURN_RIGHT,
    TURN_LEFT,
};



class Car {

public:
    int x;
    int y;
    float step = 0.5;
    float speed = 0;
    int speedLimit = 0.2f;
    
    PathHandler pathHandler;
    Direction direction;
    CarState state = CarState::FRONT;

    Car(int x, int y, Direction direction);

    void update(Game* game);
    void move();

    Vector<float> calcPosition();
};