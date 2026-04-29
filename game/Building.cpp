#include "Building.hpp"

#include <math.h>

const Vector<int> SIZES[] = {
	{3,2},
	{-1,-1}
};

Building* Building::create_home(
	int owner,
	int capacity,
	int rent
) {
	auto b = new Building;
	b->owner = owner;
	b->type = BuildingType::HOME;
	b->home.left = capacity;
	b->home.capacity = capacity;
	b->home.rent = rent;
	b->home.characters = new Character*[capacity];
	for (int i = 0; i < capacity; i++) {
		b->home.characters[i] = nullptr;
	}
	return b;
}

Building* Building::create_oilField(
	int owner,
	float crude,
	int factor,
	int size
) {
	auto b = new Building;
	b->owner = owner;
	b->type = BuildingType::OIL_FIELD;
	b->oilField.crude = crude;
	b->oilField.refined = 0;
	b->oilField.factor = expf(-1.0f / (float)factor);
	b->oilField.left = size*size;
	b->oilField.size = size;
	return b;
}



Vector<int> Building::getSize() const {
	int x = SIZES[(int)this->type].x;

	if (x >= 0)
		return {x, SIZES[(int)this->type].y};

	switch (this->type) {
	case BuildingType::OIL_FIELD:
		return {this->oilField.size, this->oilField.size};

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

	case BuildingType::OIL_FIELD:
		*ptr++ = {this->oilField.size/2 - 1, this->oilField.size - 1};
		*ptr++ = {this->oilField.size/2    , this->oilField.size - 1};
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

	case BuildingType::OIL_FIELD:
		*ptr++ = {this->oilField.size/2 - 1, 0};
		*ptr++ = {this->oilField.size/2    , 0};
		break;
		
	}

	return (int)(ptr-list);
}

#include <stdio.h>
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

	case BuildingType::OIL_FIELD:
	{
		if (this->oilField.left > 0) {
			this->oilField.left--;
			return 0;
		}
		return -1;
	}

	}


	return -1;
}

void Building::leave(int position) {
	switch (this->type) {
	case BuildingType::HOME:
	{
		if (this->home.characters[position]) {
			this->home.left++;
		}
		this->home.characters[position] = nullptr;
		break;
	}

	case BuildingType::OIL_FIELD:
	{
		this->oilField.left++;
		break;
	}
	}

}

bool Building::isFull() const {
	switch (this->type) {
	case BuildingType::HOME:
		return this->home.left == 0;

	case BuildingType::OIL_FIELD:
		return this->home.left == 0;

	}

	return true;
}




Building::~Building() {
	switch (this->type) {
	case BuildingType::HOME:
		delete[] this->home.characters;
		break;

	case BuildingType::OIL_FIELD:
		break;
	}
}
