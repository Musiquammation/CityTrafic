#pragma once

#include "declarations.hpp"
#include "Direction.hpp"

#include <unordered_map>


struct pair_hash {
    std::size_t operator()(const std::pair<int,int>& p) const noexcept {
        return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
};

class CarHandler {
    std::unordered_map<std::pair<int,int>, Car*, pair_hash> cars;

public:
    friend class Api;

    ~CarHandler();

    
    Car* spawnCar(int x, int y, Direction direction);
    void updateCars(Game* game);
    void moveCars();
    Car* getCar(int x, int y);
    void clear();

    // Iterators
    auto begin() { return this->cars.begin(); }
    auto end() { return this->cars.end(); }
    auto begin() const { return this->cars.begin(); }
    auto end() const { return this->cars.end(); }
};