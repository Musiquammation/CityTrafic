#include "Building.hpp"

#include "PanelId.hpp"
#include "Game.hpp"
#include "jobs/OilFieldJob.hpp"

#include "DebugLogger.hpp"
DebugLogger print{"Building"};


#include <math.h>
#include <stdint.h>

const Vector<int> SIZES[] = {
	{3,2},
	{-1,-1},
	{8,4},
	{3,3}
};

Building* Building::create_home(
	int owner,
	int capacity,
	int rent
) {
	auto b = new Building;
	b->owner = owner;
	b->type = BuildingType::HOME;
	b->home.leftEmployees = capacity;
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
	int size,
	int jobIdx
) {
	auto b = new Building;
	b->owner = owner;
	b->type = BuildingType::OIL_FIELD;
	b->oilField.crude = crude;
	b->oilField.refined = 0;
	b->oilField.factor = expf(-1.0f / (float)factor);
	b->oilField.leftEmployees = size*size;
	b->oilField.size = size;
	b->oilField.jobIdx = jobIdx;
	return b;
}

Building* Building::create_plantation(
	int owner,
	int delay,
	int jobIdx

) {
	auto b = new Building;
	b->owner = owner;
	b->type = BuildingType::PLANTATION;
	b->plantation.couldown = delay;
	b->plantation.delay = delay;
	b->plantation.stock = 0.0f;
	b->plantation.jobIdx = jobIdx;
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

	case BuildingType::PLANTATION:
		*ptr++ = {3, 7};
		*ptr++ = {4, 7};
		break;

	case BuildingType::GROCERY:
		*ptr++ = {2, 2};
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
		
	case BuildingType::PLANTATION:
		*ptr++ = {3, 0};
		*ptr++ = {4, 0};
		break;

	case BuildingType::GROCERY:
		*ptr++ = {0, 0};
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
				this->home.leftEmployees--;
				return i;
			}
		}
		return -1;
	}

	case BuildingType::OIL_FIELD:
	{
		if (this->oilField.leftEmployees > 0) {
			this->oilField.leftEmployees--;
			return 0;
		}
		return -1;
	}

	case BuildingType::PLANTATION:
	{
		return 0;
	}

	case BuildingType::GROCERY:
	{
		int max = this->grocery.cashierEfficiency * this->grocery.cashiers;
		if (this->grocery.clients >= max)
			return -1;

		return 0;
	}

	}


	return -1;
}

void Building::leave(int position) {
	switch (this->type) {
	case BuildingType::HOME:
	{
		if (this->home.characters[position]) {
			this->home.leftEmployees++;
		}
		this->home.characters[position] = nullptr;
		break;
	}

	case BuildingType::OIL_FIELD:
	{
		this->oilField.leftEmployees++;
		break;
	}

	case BuildingType::PLANTATION:
	{
		break;
	}

	case BuildingType::GROCERY:
	{
		this->grocery.clients--;
		break;
	}
	}

}

bool Building::isFull() const {
	switch (this->type) {
	case BuildingType::HOME:
		return this->home.leftEmployees == 0;

	case BuildingType::OIL_FIELD:
		return this->home.leftEmployees == 0;

	case BuildingType::PLANTATION:
		return false;

	case BuildingType::GROCERY:
	{
		int max = this->grocery.cashierEfficiency * this->grocery.cashiers;
		if (this->grocery.clients >= max)
			return -1;

		return 0;

	}
	}

	return true;
}




uint32_t* Building::getPanelData(const Game& game) {
	#define flt(x) ((*(uint32_t*)&x))

	switch (this->type) {
	case BuildingType::HOME:
	{
		static constexpr int COUNT = 2;
		auto result = (uint32_t*)malloc(sizeof(uint32_t)*(COUNT+2));
		result[0] = COUNT; // length (as uint32_t)
		result[1] = (uint32_t)PanelId::BUILDING_HOME;
		result[2] = this->home.rent;
		result[3] = this->home.capacity;
		return result;
	}

	case BuildingType::OIL_FIELD:
	{
		static constexpr int COUNT = 8;
		auto _job = game.getJob(this->oilField.jobIdx);
		auto& job = dynamic_cast<OilFieldJob&>(*_job);
		auto result = (uint32_t*)malloc(sizeof(uint32_t)*(COUNT+2));
		result[0] = COUNT; // length (as uint32_t)
		result[1] = (uint32_t)PanelId::BUILDING_OIL_FIELD;
		result[2] = flt(this->oilField.crude);
		result[3] = flt(this->oilField.refined);
		result[4] = job.startTime.hour;
		result[5] = job.finishTime.hour;
		result[6] = flt(job.salaryPerLiter);
		result[7] = flt(job.pricePerLiter);
		result[8] = job.employeesCounters.raffiners.current;
		result[9] = job.employeesCounters.raffiners.goal;
		return result;
	}

	case BuildingType::PLANTATION:
	{
		static constexpr int COUNT = 1;
		auto _job = game.getJob(this->oilField.jobIdx);
		auto& job = dynamic_cast<OilFieldJob&>(*_job);
		auto result = (uint32_t*)malloc(sizeof(uint32_t)*(COUNT+2));

		/// TODO: that
		result[0] = 32;
		return result;
	}

	case BuildingType::GROCERY:
	{
		static constexpr int COUNT = 5;
		auto result = (uint32_t*)malloc(sizeof(uint32_t)*(COUNT+2));
		result[0] = COUNT; // length (as uint32_t)
		result[1] = (uint32_t)PanelId::BUILDING_GROCERY;
		result[2] = flt(this->grocery.stock);
		result[3] = this->grocery.clients;
		result[4] = this->grocery.cashierEfficiency;
		result[5] = this->grocery.cashiers;
		return result;


	}


	}


	#undef flt
	return nullptr;
}

void Building::setPanelData(const uint32_t* data, Game& game) {
	#define flt(x) ((*(uint32_t*)&x))

	switch (this->type) {
	case BuildingType::HOME:
	{
		this->home.rent = data[0];
		break;
	}

	case BuildingType::OIL_FIELD:
	{
		auto _job = game.getJob(this->oilField.jobIdx);
		auto& job = dynamic_cast<OilFieldJob&>(*_job);

		job.startTime.hour = (short)(data[4]%24);
		job.finishTime.hour = (short)(data[5]%24);
		flt(job.salaryPerLiter) = data[6];
		flt(job.pricePerLiter) = data[7];
		job.employeesCounters.raffiners.current = data[8];
		job.employeesCounters.raffiners.goal = data[9];
		break;
	}

	case BuildingType::PLANTATION:
	{
		// No data to edit
		break;
	}

	case BuildingType::GROCERY:
	{
		// No data to edit
		break;

	}

	}

	#undef flt
}




Building::~Building() {
	switch (this->type) {
	case BuildingType::HOME:
		delete[] this->home.characters;
		break;

	case BuildingType::OIL_FIELD:
		break;

	case BuildingType::PLANTATION:
		break;

	case BuildingType::GROCERY:
		break;
	}
	
}
