#pragma once

#include "Vector.hpp"
#include "declarations.hpp"
#include <stdint.h>

#include "BuildingType.hpp"


struct Building {
	BuildingType type;
	int owner;

	union {
		struct {
			Character** characters;
			int left;
			int capacity;
			int rent;
		} home;

		struct {
			float crude;
			float refined;
			float factor;
			int left;
			int size;
			int jobIdx;
		} oilField;
	};
	
	
	static Building* create_home(
		int owner,
		int capacity,
		int rent
	);

	static Building* create_oilField(
		int owner,
		float crude,
		int factor,
		int size,
		int jobIdx
	);



	int enter(Character* c);
	void leave(int position);
	bool isFull() const;
	
	Vector<int> getSize() const;
	int getBufferLargeLength() const;
	int fillEntryList(Vector<int> list[]) const;
	int fillLeaveList(Vector<int> list[]) const;

	uint32_t* getPanelData();
	void setPanelData(const uint32_t* data);

	~Building();
};