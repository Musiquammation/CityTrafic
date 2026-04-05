#pragma once

#include "Vector.hpp"
#include "Direction.hpp"

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
    Direction direction;
    CarState state = CarState::FRONT;

    Car(int x, int y, Direction direction);

    void update();
    void move();

    Vector<float> calcPosition();
};