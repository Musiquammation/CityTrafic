#include "Building.hpp"

Building* Building::create_home(int capacity) {
	auto b = new Building;
	b->type = BuildingType::HOME;
	b->home.capacity = capacity;
	b->home.characters = new Character*[capacity];
	for (int i = 0; i < capacity; i++) {
		b->home.characters[i] = nullptr;
	}
	return b;
}

int Building::Home::add(Character* c) {
	for (int i = 0; i < this->capacity; i++) {
		if (this->characters[i] == nullptr) {
			this->characters[i] = c;
			this->left--;
			return i;
		}
	}
	return -1;
}

void Building::Home::remove(int position) {
	this->characters[position] = nullptr;
}

bool Building::Home::isFull() const {
	return this->left == 0;
}


const Vector<int> SIZES[] = {
	{3,2}
};

Vector<int> Building::getSize() const {
	int x = SIZES[(int)this->type].x;

	if (x >= 0)
		return {x, SIZES[(int)this->type].y};

	switch (this->type) {
	default:
		return {1,1};
	}
}


int Building::getBufferLargeLength() const {
	return 16;
}

int Building::fillEntryList(Vector<int> list[]) const {
	Vector<int>* ptr = list;
	switch (this->type) {
	case BuildingType::HOME:
		*ptr++ = {2, 1};
		break;
	}

	return (int)(ptr-list);
}


int Building::fillLeaveList(Vector<int> list[]) const {
	Vector<int>* ptr = list;
	switch (this->type) {
	case BuildingType::HOME:
		*ptr++ = {2, 2};
		break;
	}

	return (int)(ptr-list);
}




Building::~Building() {
	switch (this->type) {
	case BuildingType::HOME:
		delete[] this->home.characters;
		break;
	}
}