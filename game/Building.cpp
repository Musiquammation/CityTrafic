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

int Building::Home::remove(Character* c) {
	for (int i = 0; i < this->capacity; i++) {
		if (this->characters[i] == c) {
			this->characters[i] = nullptr;
			this->left++;
			return i;
		}
	}
	return -1;
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


int Building::fillBuildingSpecs(BuildingElementSpec list[]) const {
	BuildingElementSpec* ptr = list;
	switch (this->type) {
	case BuildingType::HOME:
		*list++ = {2, 1, true, false}; // entry (2,1)
		*list++ = {1, 0, false, true}; // exit  (1,2)
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