#pragma once

#include "declarations.hpp"

#include "Vector.hpp"
#include "Cell.hpp"


#include <stdint.h>
#include <unordered_set>



template<>
struct std::hash<Vector<int>> {
    size_t operator()(const Vector<int>& v) const noexcept {
        return (static_cast<size_t>(static_cast<uint32_t>(v.x)) << 32) |
               static_cast<uint32_t>(v.y);
    }
};

typedef struct {
    int x;
    int y;
    int width;
    int height;
} MapSize;

class Map {
    Cell* cells;    
    std::unordered_set<Vector<int>> editedCells{};
    int x;
    int y;
    int width;
    int height;


public:
    friend class Api;

    Map(int width, int height);
    ~Map();

    void expand(int left, int top, int right, int bottom);
    Cell* getEditCell(int x, int y);
    const Cell* getCell(int x, int y) const;
    MapSize getMapSize() const;

    void resetCarMarks() const;

    /**
     * First element is length,
     * then format is:
     * {dx(8), dy(8), data(16)} for each element
     */
    uint32_t* collectEditedCells(int x, int y, int width, int height);
};


extern const Cell _outCellBuffer;
