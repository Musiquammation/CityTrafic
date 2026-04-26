#pragma once

#include "declarations.hpp"

#include "Vector.hpp"
#include "Cell.hpp"

#include <map>
#include <vector>
#include <stdint.h>
#include <unordered_set>



template<>
struct std::hash<Vector<int>> {
    size_t operator()(const Vector<int>& v) const noexcept {
        return (uint64_t(v.x) << 32) | uint64_t(v.y);
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

    std::vector<std::unordered_set<Vector<int>>> editedCells{};
    std::map<Vector<int>, Building*> buildings;

    int x;
    int y;
    int width;
    int height;

    struct BuildingInfo {
        int x;
        int y;
        Building* building;
    };



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
    uint32_t* collectEditedCells(int x, int y,
        int width, int height, int layer);
    uint32_t* collectEditedCells(int layer);

    void applyEdits(const uint32_t* edits);

    int addEditedCellsLayer();
    void removeEditedCellsLayer(int layer);

    bool checkBounds(int x, int y, int width, int height) const;

    void copyCells(Cell* dst, int x, int y, int w, int h) const;

    BuildingInfo getBuilding(int x, int y);
    bool addBuilding(int x, int y, Building* building, Game& game);
    bool removeBuilding(int x, int y, Game& game);
};


extern const Cell _outCellBuffer;
