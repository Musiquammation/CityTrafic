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
		*ptr++ = {2, 0};
		break;
	}

	return (int)(ptr-list);
}


int Building::fillLeaveList(Vector<int> list[]) const {
	Vector<int>* ptr = list;
	switch (this->type) {
	case BuildingType::HOME:
		*ptr++ = {2, 1};
		break;
	}

	return (int)(ptr-list);
}



int Building::enter(Character* c) {
	switch (this->type) {
	case BuildingType::HOME:
	{
		for (int i = 0; i < this->home.capacity; i++) {
			if (this->home.characters[i] == nullptr) {
				this->home.characters[i] = c;
				this->home.left--;
				return i;
			}
		}
		return -1;
	}
	
	}


	return -1;
}

void Building::leave(int position) {
	switch (this->type) {
	case BuildingType::HOME:
		this->home.characters[position] = nullptr;
		break;
	}

}

bool Building::isFull() const {
	switch (this->type) {
	case BuildingType::HOME:
		return this->home.left == 0;
	}

	return true;
}




Building::~Building() {
	switch (this->type) {
	case BuildingType::HOME:
		delete[] this->home.characters;
		break;
	}
}
