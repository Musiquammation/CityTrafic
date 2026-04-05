#include "Array.hpp"
#include <algorithm>
#include <stdexcept>

template<typename T>
Array<T>::Array() : buffer(nullptr), reserved(0), length(0) {}

template<typename T>
Array<T>::Array(std::size_t allowed) : reserved(allowed), length(0) {
	buffer = static_cast<T*>(malloc(sizeof(T) * allowed));
}

template<typename T>
Array<T>::Array(const Array& other) {
	copyFrom(other);
}

template<typename T>
Array<T>::~Array() {
	if (buffer)
		free(buffer);
}

template<typename T>
void Array<T>::copyFrom(const Array& other) {
	reserved = other.length;
	length = other.length;
	buffer = static_cast<T*>(malloc(sizeof(T) * reserved));
	std::memcpy(buffer, other.buffer, sizeof(T) * length);
}

template<typename T>
T& Array<T>::operator[](std::size_t index) {
	if (index >= length)
		throw std::out_of_range("Array index out of range");
	return buffer[index];
}

template<typename T>
const T& Array<T>::operator[](std::size_t index) const {
	if (index >= length)
		throw std::out_of_range("Array index out of range");
	return buffer[index];
}

template<typename T>
T* Array<T>::get(std::size_t index) {
	if (index >= length) return nullptr;
	return &buffer[index];
}

template<typename T>
const T* Array<T>::get(std::size_t index) const {
	if (index >= length) return nullptr;
	return &buffer[index];
}

template<typename T>
void Array<T>::ensureCapacity(std::size_t minCapacity) {
	if (minCapacity <= reserved) return;

	std::size_t newReserved = reserved * 2 + 1;
	if (newReserved < minCapacity) newReserved = minCapacity;

	T* newBuffer = static_cast<T*>(malloc(sizeof(T) * newReserved));
	if (buffer) {
		std::memcpy(newBuffer, buffer, sizeof(T) * length);
		free(buffer);
	}
	buffer = newBuffer;
	reserved = newReserved;
}

template<typename T>
T* Array<T>::push() {
	ensureCapacity(length + 1);
	return &buffer[length++];
}

template<typename T>
T* Array<T>::reach(std::size_t index, const T& emptyValue) {
	if (index < length)
		return &buffer[index];

	ensureCapacity(index + 1);
	for (std::size_t i = length; i < index; ++i)
		buffer[i] = emptyValue;

	length = index + 1;
	return &buffer[index];
}

template<typename T>
void Array<T>::shrinkToFit() {
	if (length == reserved) return;

	T* newBuffer = static_cast<T*>(malloc(sizeof(T) * length));
	std::memcpy(newBuffer, buffer, sizeof(T) * length);
	free(buffer);
	buffer = newBuffer;
	reserved = length;
}

template<typename T>
T* Array<T>::search(const T& value, Comparator comparator) {
	for (std::size_t i = 0; i < length; ++i) {
		if (comparator(buffer[i], value) == 0)
			return &buffer[i];
	}
	return nullptr;
}

template<typename T>
T* Array<T>::binarySearch(const T& value, Comparator comparator) {
	if (length == 0) return nullptr;
	std::size_t left = 0;
	std::size_t right = length - 1;

	while (left <= right) {
		std::size_t mid = left + (right - left) / 2;
		int cmp = comparator(buffer[mid], value);
		if (cmp == 0) return &buffer[mid];
		if (cmp < 0) left = mid + 1;
		else if (mid == 0) break;
		else right = mid - 1;
	}
	return nullptr;
}

template<typename T>
T* Array<T>::binaryCompare(const T& value, Comparator comparator) {
	if (length == 0) return nullptr;
	if (comparator(buffer[0], value) >= 0) return &buffer[0];
	if (comparator(buffer[length-1], value) <= 0) return &buffer[length];

	std::size_t left = 0;
	std::size_t right = length - 1;
	while (right - left > 1) {
		std::size_t mid = left + (right - left) / 2;
		int cmp = comparator(buffer[mid], value);
		if (cmp >= 0) right = mid;
		else left = mid;
	}
	return &buffer[right];
}

template<typename T>
T* Array<T>::pushSorted(const T& value, Comparator comparator) {
	ensureCapacity(length + 1);

	if (length == 0) {
		buffer[length++] = value;
		return &buffer[0];
	}

	T* pos = binaryCompare(value, comparator);
	std::size_t index = pos - buffer;
	std::memmove(buffer + index + 1, buffer + index, (length - index) * sizeof(T));
	buffer[index] = value;
	length++;
	return &buffer[index];
}

template<typename T>
void Array<T>::sort(Comparator comparator) {
	if (length <= 1) return;
	std::sort(buffer, buffer + length, [&](const T& a, const T& b){ return comparator(a,b) < 0; });
}

template<typename T>
void Array<T>::sortAndRemoveDuplicates(Comparator comparator) {
	if (length <= 1) return;
	sort(comparator);

	std::size_t write = 1;
	for (std::size_t read = 1; read < length; ++read) {
		if (comparator(buffer[write-1], buffer[read]) != 0)
			buffer[write++] = buffer[read];
	}
	length = write;
}

