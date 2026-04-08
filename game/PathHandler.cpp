#include "PathHandler.hpp"
#include <limits.h>
#include <stdexcept>

template<>
PathHandler<true>::~PathHandler() {
	if (this->array) {
		delete this->array;
		delete[] this->dirArray;
	}
}

template<>
PathHandler<false>::~PathHandler() {

}

template<bool freeArray>
template<bool freeArraySrc>
PathHandler<freeArray>::PathHandler(const PathHandler<freeArraySrc>& src) {
	if constexpr (freeArray) {
		static_assert(freeArraySrc == false, "This constructor is disabled for PathHandler<true>");

	} else {
		this->array = src.array;
		this->dirArray = src.dirArray;
		this->step = src.step;
		this->length = src.length;
	}
}

template<>
void PathHandler<true>::fill(Vector<int>* array, uint8_t* bitArray, int length) {
	this->array = array;
	this->dirArray = bitArray;
	this->length = length;
	this->step = 0;
}

template<>
void PathHandler<false>::fill(Vector<int>* array, uint8_t* bitArray, int length) {
		static_assert(1, "fill() is disabled for PathHandler<true>");
}

template<bool freeArray>
Vector<int> PathHandler<freeArray>::seek() {
	if (!this->array || this->step < 0)
		return {INT_MAX, INT_MAX};
		
	return this->array[this->step];
}

template<bool freeArray>
Direction PathHandler<freeArray>::seekDirection() {
	#if TESTING
	if (!this->array || this->step < 0)
		throw std::out_of_range("Step index out of range or array is null in seekRight");

	#endif

	int bitIndex = 2 * step;
	int byteIndex = bitIndex / 8;
	int bitInByte = bitIndex % 8;

	return (Direction)((dirArray[byteIndex] >> bitInByte) & 0b11);
}

template<bool freeArray>
void PathHandler<freeArray>::next() {
	if (this->step < this->length) {
		this->step++;
	} else {
		this->step = -1;
	}
}



template PathHandler<false>::PathHandler(const PathHandler<true>&);