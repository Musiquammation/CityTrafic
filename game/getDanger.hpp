#pragma once

#include "declarations.hpp"
#include "Direction.hpp"

typedef struct {
    float speed;
} getDanger_t;


getDanger_t getDanger(const Car* car, Game* game);