#include "Map.hpp"
#include "Cell.hpp"
#include <cstdlib>
#include <cstring>

Map::Map(int width, int height)
	: x(0), y(0), w(width), h(height) {
	cells = (Cell*)malloc(sizeof(Cell) * w * h);
}

Map::~Map() {
	if (cells) {
		free(cells);
		cells = nullptr;
	}
}

void Map::expand(int newX, int newY, int newW, int newH) {
	int totalW = newX + newW;
	int totalH = newY + newH;

	Cell* newCells = (Cell*)malloc(sizeof(Cell) * totalW * totalH);
	if (!newCells) return;

	memset(newCells, 0, sizeof(Cell) * totalW * totalH);

	for (int j = 0; j < this->h; ++j) {
		for (int i = 0; i < this->w; ++i) {
			int oldIndex = j * this->w + i;
			int newIndex = (j + newY) * totalW + (i + newX);
			newCells[newIndex] = this->cells[oldIndex];
		}
	}

	free(this->cells);
	this->cells = newCells;
	this->x = newX;
	this->y = newY;
	this->w = totalW;
	this->h = totalH;
}

Cell* Map::getCell(int cx, int cy) const {
	if (cx < x || cx >= x + w || cy < y || cy >= y + h)
		return nullptr;
	return &cells[(cy - y) * w + (cx - x)];
}

MapSize Map::getMapSize() const {
	return (MapSize){this->x, this->y, this->w, this->h};
}