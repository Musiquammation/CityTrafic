#pragma once

#include "Vector.hpp"
#include "declarations.hpp"

enum class BuildingType: int {
	HOME,
};


struct Building {
	BuildingType type;

	struct Home {
		Character** characters;
		int left;
		int capacity;

		int add(Character* c);
		void remove(int position);
		bool isFull() const;
	};


	union {
		Home home;
	};
	
	
	static Building* create_home(int capacity);


	
	Vector<int> getSize() const;
	int getBufferLargeLength() const;
	int fillEntryList(Vector<int> list[]) const;
	int fillExitList(Vector<int> list[]) const;

	~Building();
};