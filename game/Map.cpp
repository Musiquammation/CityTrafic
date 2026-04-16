#include "Map.hpp"
#include "Cell.hpp"
#include <cstdlib>
#include <cstring>

#include <stdio.h>
#include "utils/mfor.hpp"

#include <unordered_map>

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

	for (auto& line : this->editedCells) {
		line.insert(Vector<int>{x,y});
	}
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


uint32_t* Map::collectEditedCells(
	int x, int y, int width, int height, int layer
) {
	struct Region {
		int32_t x;
		int32_t y;
		std::vector<uint32_t> cells;
	};

	auto& edited = this->editedCells[layer];
	// Group in regions
	std::unordered_map<uint64_t, Region> regions;

	auto makeKey = [](int32_t x, int32_t y) -> uint64_t {
		return (uint64_t(uint32_t(x)) << 32) | uint32_t(y);
	};

	for (const auto& pos : edited) {
		if (!(pos.x >= x && pos.x < x + width &&
			pos.y >= y && pos.y < y + height))
			continue;

		// Origin
		int32_t rx = pos.x & ~0xFF;
		int32_t ry = pos.y & ~0xFF;

		uint64_t key = makeKey(rx, ry);
		auto& region = regions[key];

		region.x = rx;
		region.y = ry;

		uint8_t dx = static_cast<uint8_t>(pos.x - rx);
		uint8_t dy = static_cast<uint8_t>(pos.y - ry);

		const Cell* cell = getCell(pos.x, pos.y);
		uint16_t data = cell->data;

		uint32_t packed =
			(uint32_t(dx) << 24) |
			(uint32_t(dy) << 16) |
			uint32_t(data);

		region.cells.push_back(packed);
	}

	// Get size
	uint32_t totalSize = 2; // totalSize, regionsLength

	for (const auto& [_, region] : regions) {
		totalSize += 3; // x, y, length
		totalSize += (uint32_t)region.cells.size();
	}

	// Write in buffer
	uint32_t* buffer = (uint32_t*)malloc(sizeof(uint32_t) * totalSize);
	uint32_t* ptr = buffer;
	*ptr++ = totalSize;
	*ptr++ = (uint32_t)regions.size();

	for (const auto& [_, region] : regions) {
		*ptr++ = region.x;
		*ptr++ = region.y;
		*ptr++ = (uint32_t)region.cells.size();

		for (uint32_t v : region.cells) {
			*ptr++ = v;
		}
	}


	// Delete used cells
	for (auto it = edited.begin(); it != edited.end(); ) {
		if (it->x >= x && it->x < x + width &&
			it->y >= y && it->y < y + height) {
			it = edited.erase(it);
		} else {
			++it;
		}
	}

	return buffer;
}



uint32_t* Map::collectEditedCells(int layer) {
	auto& edited = this->editedCells[layer];


	// Count how many edited cells are inside the requested region
	uint32_t count = (uint32_t)edited.size();

	// Allocate buffer: 1 for count + N packed cells
	uint32_t* buffer = (uint32_t*)malloc(sizeof(uint32_t) * (1 + count));
	if (!buffer) return nullptr;

	buffer[0] = count;

	uint32_t* ptr = buffer + 1;

	// Fill buffer and remove collected points from the set
	for (auto& pos: edited) {
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
	}

	// Clean set
	edited.clear();

	return buffer;
}



void Map::applyEdits(const uint32_t* edits) {
	#define take() (*edits++)

	for (int regionsCount = take(); regionsCount; regionsCount--) {
		int x = (int)take();
		int y = (int)take();

		for (uint32_t count = take(); count; count--) {
			auto line = take();

			uint8_t dx = (line >> 24) & 0xff;
			uint8_t dy = (line >> 16) & 0xff;
			uint16_t data = line & 0xffff;

			this->getEditCell(x + dx, y + dy)->data = data;
		}
	}


	

	#undef take
}



int Map::addEditedCellsLayer() {
	this->editedCells.emplace_back();
	return (int)this->editedCells.size() - 1;
}

void Map::removeEditedCellsLayer(int layer) {
	if (layer >= 0 && layer < (int)this->editedCells.size()) {
		this->editedCells.erase(this->editedCells.begin() + layer);
	}
}



bool Map::checkBounds(int x, int y, int width, int height) const {
	return (x >= this->x && x + width <= this->x + this->width &&
		y >= this->y && y + height <= this->y + this->height);
}

