#include "Map.hpp"
#include "Cell.hpp"
#include <cstdlib>
#include <cstring>

Map::Map(int width, int height)
	: x(0), y(0), width(width), height(height) {

	this->cells = (Cell*)malloc(sizeof(Cell) * width * height);
	this->carIds = (carId_t*)malloc(sizeof(carId_t) * width * height);
}

Map::~Map() {
	free(this->cells);
	free(this->carIds);
}

void Map::expand(int x, int y, int right, int bottom) {
	int totalW = right - x;
	int totalH = bottom - y;

	Cell* newCells = (Cell*)malloc(sizeof(Cell) * totalW * totalH);
	carId_t* newIds = (carId_t*)malloc(sizeof(carId_t) * totalW * totalH);

	memset(newCells, 0, sizeof(Cell) * totalW * totalH);

	int w = this->width;
	int h = this->height;
	for (int j = 0; j < h; ++j) {
		for (int i = 0; i < w; ++i) {
			int oldIndex = j * w + i;
			int newIndex = (j - y) * totalW + (i - x);
			newCells[newIndex] = this->cells[oldIndex];
			newIds[newIndex] = this->carIds[oldIndex];
		}
	}

	free(this->cells);
	free(this->carIds);

	this->cells = newCells;
	this->carIds = newIds;
	this->x = x;
	this->y = y;
	this->width = right - x;
	this->height = bottom - y;
}

Cell* Map::getCell(int cx, int cy) const {
	return &cells[(cy - this->y) * this->width + (cx - this->x)];
}

MapSize Map::getMapSize() const {
	return (MapSize){this->x, this->y, this->width, this->height};
}