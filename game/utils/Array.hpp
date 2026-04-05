#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>

template<typename T>
class Array {
public:
	using Comparator = std::function<int(const T&, const T&)>;

	Array();
	explicit Array(std::size_t allowed);
	Array(const Array& other);
	~Array();

	// Capacity
	std::size_t size() const { return length; }
	bool empty() const { return length == 0; }

	// Access
	T& operator[](std::size_t index);
	const T& operator[](std::size_t index) const;

	T* get(std::size_t index);
	const T* get(std::size_t index) const;

	// Modifiers
	T* push();
	T* pushSorted(const T& value, Comparator comparator);
	T* reach(std::size_t index, const T& emptyValue);
	void shrinkToFit();
	void copyFrom(const Array& other);

	// Search
	T* search(const T& value, Comparator comparator);
	T* binarySearch(const T& value, Comparator comparator);
	T* binaryCompare(const T& value, Comparator comparator);

	// Sort
	void sort(Comparator comparator);
	void sortAndRemoveDuplicates(Comparator comparator);

	// Iterators
	T* data() { return buffer; }
	const T* data() const { return buffer; }

private:
	T* buffer = nullptr;
	std::size_t reserved = 0;
	std::size_t length = 0;

	void ensureCapacity(std::size_t minCapacity);
};

