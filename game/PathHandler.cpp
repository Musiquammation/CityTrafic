#include "PathHandler.hpp"
#include <limits.h>
#include <stdexcept>


PathHandler::~PathHandler() {
	delete this->array;
	delete[] this->bitArray;
}

void PathHandler::fill(Vector<int>* array, uint8_t* bitArray, int length) {
	this->array = array;
	this->bitArray = bitArray;
	this->length = length;
	this->step = 0;
}

Vector<int> PathHandler::seek() {
	if (!this->array || this->step < 0)
		return {INT_MAX, INT_MAX};
		
	return this->array[this->step];
}

bool PathHandler::seekIsRight() {
	#if TESTING
	if (!this->array || this->step < 0)
		throw std::out_of_range("Step index out of range or array is null in seekRight");

	#endif

	int byteIndex = this->step / 8;
	int bitInByte = this->step % 8;
	return (this->bitArray[byteIndex] >> bitInByte) & 1;

}

void PathHandler::next() {
	if (this->step < this->length) {
		this->step++;
	} else {
		this->step = -1;
	}
}