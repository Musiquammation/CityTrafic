#include "Cell.hpp"
#include "Game.hpp"

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


void Cell::setType(CellType type, Game& game, cell_t arg) {
	cell_t current = this->data;
	cell_t result;

	switch ((CellType)(current & 0x0f)) {
	case CellType::NONE:
		result = 0;
		break;

	case CellType::ROAD:
		result = 0;
		break;

	default:
		result = 0;
		break;
	}

	this->data = result | ((int)type & 0x0f);
}

