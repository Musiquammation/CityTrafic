#include "PathHandler.hpp"
#include <limits.h>
#include <stdexcept>


PathHandler::~PathHandler() {
	delete this->array;
	delete[] this->dirArray;
}

void PathHandler::fill(Vector<int>* array, uint8_t* bitArray, int length) {
	this->array = array;
	this->dirArray = bitArray;
	this->length = length;
	this->step = 0;
}

Vector<int> PathHandler::seek() {
	if (!this->array || this->step < 0)
		return {INT_MAX, INT_MAX};
		
	return this->array[this->step];
}

Direction PathHandler::seekDirection() {
	#if TESTING
	if (!this->array || this->step < 0)
		throw std::out_of_range("Step index out of range or array is null in seekRight");

	#endif

	int bitIndex = 2 * step;
	int byteIndex = bitIndex / 8;
	int bitInByte = bitIndex % 8;

	return (Direction)((dirArray[byteIndex] >> bitInByte) & 0b11);
}

void PathHandler::next() {
	if (this->step < this->length) {
		this->step++;
	} else {
		this->step = -1;
	}
}