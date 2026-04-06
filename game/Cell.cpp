#include "Cell.hpp"

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
	cell_t type = this->data & 0x0f;

	#ifdef TESTING
	if (type >= (cell_t)CellType::COUNT) {
		throw std::out_of_range("Invalid CellType id");
	}
	#endif

	return (CellType)type;
}

void Cell::setType(CellType type) {
	this->data = (this->data & 0xf0) | ((int)type & 0x0f);
}