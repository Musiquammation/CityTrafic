#pragma once

#include "declarations.hpp"
#include "Direction.hpp"
#include "Vector.hpp"

typedef struct {
    float acceleration;
    Vector<int> targetPoint;
} getDanger_t;


getDanger_t getDanger(const Car* car, Game* game);