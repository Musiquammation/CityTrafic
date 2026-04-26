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


cell_t Cell::editType(
	CellType type,
	Game& game,
	cell_t arg,
	char* failure
) const {
	cell_t current = this->data;
	cell_t result;

	switch ((CellType)(current & 0x0f)) {
	case CellType::NONE:
		result = 0;
		break;

	case CellType::ROAD:
		result = 0;
		break;

	case CellType::BUILDING:
	case CellType::LINK:
		if (type != CellType::NONE) {
			*failure = 1;
			break;
		}
		result = arg;
		break;

	default:
		result = 0;
		break;
	}

	return result | ((int)type & 0x0f);
}

bool Cell::setType(CellType type, Game& game, cell_t arg) {
	char fail = 0;
	cell_t data = this->editType(type, game, arg, &fail);

	if (fail)
		return false;

	this->data = data;
	return true;
}
