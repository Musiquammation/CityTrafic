#pragma once

#include "Vector.hpp"
#include "declarations.hpp"

enum class BuildingType: int {
	HOME,
	OIL_FIELD,
};


struct Building {
	BuildingType type;


	union {
		struct {
			Character** characters;
			int left;
			int capacity;	
		} home;

		struct {
			float crude;
			float refined;
			float factor;
			int left;
			int size;
		} oilField;
	};
	
	
	static Building* create_home(int capacity);
	static Building* create_oilField(
		float crude, int factor, int size);



	int enter(Character* c);
	void leave(int position);
	bool isFull() const;
	
	Vector<int> getSize() const;
	int getBufferLargeLength() const;
	int fillEntryList(Vector<int> list[]) const;
	int fillLeaveList(Vector<int> list[]) const;

	~Building();
};