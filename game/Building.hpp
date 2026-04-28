#pragma once

#include "Vector.hpp"
#include "declarations.hpp"

enum class BuildingType: int {
	HOME,
};


struct Building {
	BuildingType type;


	union {
		struct {
			Character** characters;
			int left;
			int capacity;	
		} home;
	};
	
	
	static Building* create_home(int capacity);



	int enter(Character* c);
	void leave(int position);
	bool isFull() const;
	
	Vector<int> getSize() const;
	int getBufferLargeLength() const;
	int fillEntryList(Vector<int> list[]) const;
	int fillLeaveList(Vector<int> list[]) const;

	~Building();
};