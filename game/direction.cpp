#include "direction.hpp"

#include <stdint.h>
#include <stdbool.h>

int direction::getSide(uint16_t data, int direction) {
	int dir1 = (data >> 10) & 0x3;
	int dir2 = (data >> 12) & 0x3;
	int side1 = (data >> 4) & 0x7;
	int side2 = (data >> 7) & 0x7;

	if (side1 != 0 && dir1 == direction) return side1;
	if (side2 != 0 && dir2 == direction) return side2;

	return 0;
}

uint16_t direction::setSide(uint16_t data, int direction, int value) {
	int dir1 = (data >> 10) & 0x3;
	int dir2 = (data >> 12) & 0x3;
	int side1 = (data >> 4) & 0x7;
	int side2 = (data >> 7) & 0x7;

	if (side1 != 0 && dir1 == direction) {
		data &= (uint16_t)(~(0x7 << 4));
		data |= (uint16_t)((value & 0x7) << 4);
		return data;
	}
	if (side2 != 0 && dir2 == direction) {
		data &= (uint16_t)(~(0x7 << 7));
		data |= (uint16_t)((value & 0x7) << 7);
		return data;
	}

	if (side1 == 0) {
		data &= (uint16_t)(~(0x7 << 4));
		data &= (uint16_t)(~(0x3 << 10));
		data |= (uint16_t)((value & 0x7) << 4);
		data |= (uint16_t)((direction & 0x3) << 10);
		return data;
	}
	if (side2 == 0) {
		data &= (uint16_t)(~(0x7 << 7));
		data &= (uint16_t)(~(0x3 << 12));
		data |= (uint16_t)((value & 0x7) << 7);
		data |= (uint16_t)((direction & 0x3) << 12);
		return data;
	}

	return 0;	
}