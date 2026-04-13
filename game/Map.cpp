#include "Map.hpp"
#include "Cell.hpp"
#include <cstdlib>
#include <cstring>

#include "utils/mfor.hpp"

const Cell _outCellBuffer = {.data = 0};

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

Cell* Map::getEditCell(int x, int y) {
	#if TESTING
	if (x < this->x || x >= this->x + this->width || y < this->y || y >= this->y + this->height) {
		throw std::range_error{"Cell coordinates out of range"};
	}
	#endif

	this->editedCells.insert(Vector<int>{x,y});
	return &this->cells[(y - this->y) * this->width + (x - this->x)];
}


const Cell* Map::getCell(int x, int y) const {
	if (x < this->x || x >= this->x + this->width || y < this->y || y >= this->y + this->height) {
		return &_outCellBuffer;
	}
	
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


uint32_t* Map::collectEditedCells(int x, int y, int width, int height) {
    // Count how many edited cells are inside the requested region
    uint32_t count = 0;

    for (const auto& pos : editedCells) {
        if (pos.x >= x && pos.x < x + width &&
            pos.y >= y && pos.y < y + height) {
            count++;
        }
    }

    // Allocate buffer: 1 for count + N packed cells
    uint32_t* buffer = (uint32_t*)malloc(sizeof(uint32_t) * (1 + count));
    if (!buffer) return nullptr;

    buffer[0] = count;

    uint32_t* ptr = buffer + 1;

    // Fill buffer and remove collected points from the set
    for (auto it = editedCells.begin(); it != editedCells.end(); ) {
        const auto& pos = *it;

        if (pos.x >= x && pos.x < x + width &&
            pos.y >= y && pos.y < y + height
		) {

            uint8_t dx = static_cast<uint8_t>(pos.x - x);
            uint8_t dy = static_cast<uint8_t>(pos.y - y);

            const Cell* cell = getCell(pos.x, pos.y);

            // Pack cell data into 16 bits (adapt if needed)
            uint16_t data = cell->data;

            uint32_t packed =
                (static_cast<uint32_t>(dx) << 24) |
                (static_cast<uint32_t>(dy) << 16) |
                static_cast<uint32_t>(data);

            *ptr++ = packed;

            // Erase the processed point from the set
            it = editedCells.erase(it);
        } else {
            it++;
        }
    }

    return buffer;
}