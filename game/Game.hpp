#pragma once

#include "Map.hpp"
#include "CarHandler.hpp"

class Game {
public:
    Map map{32,32};
    CarHandler carHandler{};
};