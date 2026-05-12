#pragma once

#include "Vector.hpp"
#include "declarations.hpp"
#include <stdint.h>
#include <unordered_map>
#include <vector>

#include "BuildingType.hpp"

class TruckJob;

struct Building {
	BuildingType type;
	int owner;

	#if TESTING_SERV
	bool hasBeenDestroyed = false;
	#endif

	union {
		struct {
			Character** characters;
			int left;
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
			int cooldown;
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

		struct {
			ConstructionJob* job;
			Building* goal;
			int completion;
			int total;
		} construction;

		struct {
			TruckJob* job;
		} warehouse;
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

	static Building* create_construction(
		ConstructionJob* job,
		Building* building,
		int owner
	);

	static Building *create_warehouse(
		TruckJob *job,
		int owner
	);


	int enter(Character* c);
	void leave(int position);
	bool isFull() const;
	
	Vector<int> getSize() const;
	bool isSizeConstant() const;
	int getBufferLargeLength() const;
	int fillEntryList(Vector<int> list[]) const;
	int fillLeaveList(Vector<int> list[]) const;

	int getConstructionCost() const;

	uint32_t* getPanelData(const Game& game);
	bool setPanelData(const uint32_t* data, Game& game);

	Job* getJob();

	void destroy(Game& game);


	bool home_addCharacter(Character* c);
	bool home_removeCharacter(Character* c);

	void fileSave(WriteStream& stream) const;
	void fileLoad(ReadStream& stream, const std::unordered_map<Job*, Job*>& jobs);

	bool fillTruckImports(
		const Map& map,
		Vector<int> loc,
		std::vector<Vector<int>>& importList
	) const;

	~Building();
};
