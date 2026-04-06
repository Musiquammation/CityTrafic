#include "PathHandler.hpp"


PathHandler::~PathHandler() {
	if (this->array) {
		delete[] this->array;
	}
}

void PathHandler::fill(PathPoint* newArray, int newLength) {
	if (this->array) {
		delete[] this->array;
	}

	this->array = newArray;
	this->length = newLength;
	this->step = 0;
}

PathPoint* PathHandler::seek() {
	if (!this->array || this->step >= this->length) {
		return nullptr;
	}
	return &this->array[this->step];
}

void PathHandler::next() {
	if (this->step < this->length) {
		this->step++;
	}
}