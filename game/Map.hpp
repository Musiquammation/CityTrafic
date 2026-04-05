#pragma once

#include "declarations.hpp"

#include <stdint.h>
typedef uint8_t carId_t;


typedef struct {
    int x;
    int y;
    int width;
    int height;
} MapSize;

class Map {
    Cell* cells;
    carId_t* carIds;
    int x;
    int y;
    int width;
    int height;

public:
    Map(int width, int height);
    ~Map();

    void expand(int left, int top, int right, int bottom);
    Cell* getCell(int x, int y) const;
    carId_t* getCarId(int x, int y) const;
    MapSize getMapSize() const;
};