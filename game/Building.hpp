#pragma once

#include "Vector.hpp"
#include "declarations.hpp"

enum class BuildingType: int {
	HOME,
};

struct BuildingElementSpec {
	int dx;
	int dy;
	bool entry;
	bool exit;
};

struct Building {
	BuildingType type;

	struct Home {
		Character** characters;
		int left;
		int capacity;

		int add(Character* c);
		void remove(int position);
	};


	union {
		Home home;
	};
	
	
	static Building* create_home(int capacity);


	
	Vector<int> getSize() const;
	int fillBuildingSpecs(BuildingElementSpec list[]) const;

	~Building();
};