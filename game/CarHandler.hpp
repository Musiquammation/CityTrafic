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
    ~CarHandler();

    
    Car* spawnCar(int x, int y, Direction direction);
    void updateCars();
    void moveCars();
    Car* getCar(int x, int y);

    // Iterators
    auto begin() { return cars.begin(); }
    auto end() { return cars.end(); }
    auto begin() const { return cars.begin(); }
    auto end() const { return cars.end(); }
};