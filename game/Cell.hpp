#pragma once

#include <stdexcept>
#include <stdint.h>
#include "CellType.hpp"
#include "cell_t.hpp"
#include "declarations.hpp"


struct Cell {
	cell_t data = 0;

	void setCarOn();
	void setCarOff();
	bool hasCar() const;

	CellType getType() const;
	cell_t editType(CellType type, Game& game,
		cell_t arg, char* failure) const;
		
	bool setType(CellType type, Game& game, cell_t arg = 0);
};
