#pragma once

#include <stdexcept>
#include <stdint.h>
#include "CellType.hpp"

/**
 * General format:
 * 
 * +00: type
 * +04: data
 * +15: hasCar?
 * +16
 */
typedef uint16_t cell_t;

struct Cell {
	cell_t data;


	inline void setCarOn() {
		this->data |= 1<<15;
	}

	inline void setCarOff() {
		this->data &= ~(1<<15);
	}

	inline uint16_t hasCar() {
		return this->data & (1<<15);
	}

	inline CellType getType() {
		cell_t type = this->data & 0xf;

		#ifdef TESTING
		if (type >= (cell_t)CellType::COUNT) {
			throw std::out_of_range("Invalid id");
		}
		#endif

		return (CellType)type;
	}
};
