#include "direction.hpp"

#include <stdint.h>
#include <stdbool.h>

int direction::getSide(uint16_t data, int direction) {
	int dir1 = (data >> 10) & 0x3;
	int dir2 = (data >> 12) & 0x3;

	if (direction == dir1) {
		return (data >> 4) & 0x7;
	} 
	else if (direction == dir2) {
		return (data >> 7) & 0x7;
	}

	return -1;
}

uint16_t direction::setSide(uint16_t data, int direction, int value) {
	// Extract existing directions
	int dir1 = (data >> 10) & 0x3;
	int dir2 = (data >> 12) & 0x3;
	
	// Extract existing side values to check if slots are empty (0)
	int side1 = (data >> 4) & 0x7;
	int side2 = (data >> 7) & 0x7;

	// --- Update existing slot ---
	if (side1 != 0 && dir1 == direction) {
		// Clear old side bits (+04) and set new value
		data &= ~(0x7 << 4); 
		data |= (uint16_t)((value & 0x7) << 4);
		return data;
	} else if (side2 != 0 && dir2 == direction) {
		// Clear old side bits (+07) and set new value
		data &= ~(0x7 << 7);
		data |= (uint16_t)((value & 0x7) << 7);
		return data;
	}

	// --- Fill empty slot if direction doesn't exist ---
	if (side1 == 0) {
		// Set side at +04 and direction at +10
		data |= (uint16_t)((value & 0x7) << 4);
		data |= (uint16_t)((direction & 0x3) << 10);
		return data;
	} else if (side2 == 0) {
		// Set side at +07 and direction at +12
		data |= (uint16_t)((value & 0x7) << 7);
		data |= (uint16_t)((direction & 0x3) << 12);
		return data;
	}

	return 0; // Failure
}