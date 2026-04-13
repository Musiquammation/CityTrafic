#pragma once

#include "declarations.hpp"

#include "Cell.hpp"
#include <stdint.h>


typedef struct {
    int x;
    int y;
    int width;
    int height;
} MapSize;

class Map {
    Cell* cells;
    Cell outCell{.data = 0};
    int x;
    int y;
    int width;
    int height;

public:
    friend class Api;

    Map(int width, int height);
    ~Map();

    void expand(int left, int top, int right, int bottom);
    Cell* getEditCell(int x, int y) const;
    const Cell* getCell(int x, int y) const;
    MapSize getMapSize() const;

    void resetCarMarks() const;
};