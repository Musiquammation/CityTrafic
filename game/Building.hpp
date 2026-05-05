#pragma once

#include "Vector.hpp"
#include "declarations.hpp"
#include <stdint.h>

#include "BuildingType.hpp"

struct Building {
	BuildingType type;
	int owner;

	#if TESTING_SERV
	bool hasBeenDestroyed = false;
	#endif

	union {
		struct {
			Character** characters;
			int leftEmployees;
			int capacity;
			int rent;
		} home;

		struct {
			OilFieldJob* job;
			float crude;
			float refined;
			float factor;
			int leftEmployees;
			int size;
		} oilField;

		struct {
			AgricultorJob* job;
			int couldown;
			int delay;
			float stock;
		} plantation;

		struct {
			CashierJob* job;
			float stock;
			int clients;
			float cashierEfficiency;
			int cashiers;
		} grocery;
	};
	
	
	static Building* create_home(
		int owner,
		int capacity,
		int rent
	);

	static Building* create_oilField(
		OilFieldJob* job,
		int owner,
		float crude,
		int factor,
		int size
	);

	static Building* create_plantation(
		AgricultorJob* job,
		int owner,
		int delay
	);

	static Building* create_grocery(
		CashierJob* job,
		int owner
	);



	int enter(Character* c);
	void leave(int position);
	bool isFull() const;
	
	Vector<int> getSize() const;
	int getBufferLargeLength() const;
	int fillEntryList(Vector<int> list[]) const;
	int fillLeaveList(Vector<int> list[]) const;

	uint32_t* getPanelData(const Game& game);
	void setPanelData(const uint32_t* data, Game& game);

	Job* getJob();

	void destroy(Game& game);
	~Building();
};