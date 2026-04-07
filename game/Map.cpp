#include "Map.hpp"
#include "Cell.hpp"
#include <cstdlib>
#include <cstring>

#include "utils/mfor.hpp"

Map::Map(int width, int height)
	: x(0), y(0), width(width), height(height) {

	this->cells = (Cell*)calloc(width * height, sizeof(Cell));
}

Map::~Map() {
	free(this->cells);
}

void Map::expand(int x, int y, int right, int bottom) {
	int totalW = right - x;
	int totalH = bottom - y;

	Cell* newCells = (Cell*)calloc(totalW * totalH, sizeof(Cell));


	int w = this->width;
	int h = this->height;
	for (int j = 0; j < h; ++j) {
		for (int i = 0; i < w; ++i) {
			int oldIndex = j * w + i;
			int newIndex = (j - y) * totalW + (i - x);
			newCells[newIndex] = this->cells[oldIndex];
		}
	}

	free(this->cells);

	this->cells = newCells;
	this->x = x;
	this->y = y;
	this->width = right - x;
	this->height = bottom - y;
}

Cell* Map::getCell(int x, int y) const {
	#if TESTING
	if (x < this->x || x >= this->x + this->width || y < this->y || y >= this->y + this->height) {
		throw std::range_error{"Cell coordinates out of range"};
	}

	#endif
	
	return &this->cells[(y - this->y) * this->width + (x - this->x)];
}



MapSize Map::getMapSize() const {
	return (MapSize){this->x, this->y, this->width, this->height};
}

void Map::resetCarMarks() const {
	mfor(this->cells, this->width * this->height, c) {
		c->setCarOff();
	}
}
