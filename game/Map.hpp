#pragma once

#include "declarations.hpp"


typedef struct {
    int x;
    int y;
    int w;
    int h;
} MapSize;

class Map {
    Cell* cells;
    int x;
    int y;
    int w;
    int h;

public:
    Map(int width, int height);
    ~Map();

    void expand(int x, int y, int w, int h);
    Cell* getCell(int x, int y) const;
    MapSize getMapSize() const;
};