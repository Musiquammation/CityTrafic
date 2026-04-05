#pragma once

#include "declarations.hpp"

#include <stdint.h>


typedef struct {
    int x;
    int y;
    int width;
    int height;
} MapSize;

class Map {
    Cell* cells;
    int x;
    int y;
    int width;
    int height;

public:
    Map(int width, int height);
    ~Map();

    void expand(int left, int top, int right, int bottom);
    Cell* getCell(int x, int y) const;
    MapSize getMapSize() const;

    void resetCarMarks() const;
};