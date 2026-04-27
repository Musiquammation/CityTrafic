#include "Cell.hpp"
#include "Game.hpp"
#include "CellInstruction.hpp"

#include <stdio.h>

void Cell::setCarOn() {
	this->data |= 1 << 15;
}

void Cell::setCarOff() {
	this->data &= ~(1 << 15);
}

bool Cell::hasCar() const {
	return (this->data & (1 << 15)) != 0;
}

CellType Cell::getType() const {
	cell_t type = this->data & 0xf;

	#ifdef TESTING
	if (type >= (cell_t)CellType::COUNT) {
		throw std::out_of_range("Invalid CellType id");
	}
	#endif

	return (CellType)type;
}


cell_t Cell::editType(
	CellType type,
	Game& game,
	cell_t arg,
	char* failure
) const {
	cell_t current = this->data;
	cell_t result;

	#define check(ins) if (\
		type != CellType::NONE ||\
		(arg>>4) != (cell_t)ins) {\
			*failure = 1;\
			break;\
		}



	switch ((CellType)(current & 0x0f)) {
	case CellType::NONE:
		result = arg;
		break;

	case CellType::ROAD:
		result = arg;
		break;

	case CellType::BUILDING:
	case CellType::LINK:
		check(CellInstruction::BUILDING);
		result = 0;
		break;

	default:
		result = arg;
		break;
	}

	return result | ((int)type & 0x0f);

	#undef check
}

bool Cell::setType(CellType type, Game& game, cell_t arg) {
	char fail = 0;
	cell_t data = this->editType(type, game, arg, &fail);

	if (fail)
		return false;

	this->data = data;
	return true;
}
