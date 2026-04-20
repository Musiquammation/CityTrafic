#pragma once

#include <stdexcept>
#include <stdint.h>
#include "CellType.hpp"
#include "declarations.hpp"

typedef uint16_t cell_t;

struct Cell {
	cell_t data = 0;

	void setCarOn();
	void setCarOff();
	bool hasCar() const;

	CellType getType() const;
	void setType(CellType type, Game& game, cell_t arg = 0);
};
