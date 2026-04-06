#pragma once

#include <stdexcept>
#include <stdint.h>
#include "CellType.hpp"

typedef uint16_t cell_t;

struct Cell {
	cell_t data;

	void setCarOn();
	void setCarOff();
	bool hasCar() const;

	CellType getType() const;
	void setType(CellType type);
};
