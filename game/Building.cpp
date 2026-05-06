#include "Building.hpp"

#include "PanelId.hpp"
#include "Game.hpp"
#include "Character.hpp"

#include "jobs/OilFieldJob.hpp"
#include "jobs/AgricultorJob.hpp"
#include "jobs/CashierJob.hpp"
#include "jobs/ConstructionJob.hpp"

#include "DebugLogger.hpp"
DebugLogger print{"Building"};


#include <math.h>
#include <stdint.h>

const Vector<int> SIZES[] = {
	{3,2},
	{-1,-1},
	{8,4},
	{3,3},
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
	b->home.left = 0;
	b->home.capacity = capacity;
	b->home.rent = rent;
	b->home.characters = new Character*[capacity];
	for (int i = 0; i < capacity; i++) {
		b->home.characters[i] = nullptr;
	}
	return b;
}

Building* Building::create_oilField(
	OilFieldJob* job,
	int owner,
	float crude,
	int factor,
	int size
) {
	auto b = new Building;
	b->owner = owner;
	b->type = BuildingType::OIL_FIELD;
	b->oilField.job = job;
	b->oilField.crude = crude;
	b->oilField.refined = 0;
	b->oilField.factor = expf(-1.0f / (float)factor);
	b->oilField.leftEmployees = size*size;
	b->oilField.size = size;
	return b;
}

Building* Building::create_plantation(
	AgricultorJob* job,
	int owner,
	int delay

) {
	auto b = new Building;
	b->owner = owner;
	b->type = BuildingType::PLANTATION;
	b->plantation.job = job;
	b->plantation.couldown = delay;
	b->plantation.delay = delay;
	b->plantation.stock = 0.0f;
	return b;
}

Building* Building::create_grocery(
	CashierJob* job,
	int owner
) {
	auto b = new Building;
	b->owner = owner;
	b->type = BuildingType::GROCERY;
	b->grocery.job = job;
	b->grocery.stock = 0.0f;
	b->grocery.clients = 0;
	b->grocery.cashierEfficiency = 1;
	b->grocery.cashiers = 0;
	return b;
}

Building* Building::create_construction(
	ConstructionJob* job,
	Building* building,
	int owner
) {
	auto b = new Building;
	b->owner = owner;
	b->type = BuildingType::CONSTRUCTION;
	b->construction.job = job;
	b->construction.goal = building;
	b->construction.completion = 0;
	b->construction.total = building->getConstructionCost();
	return b;
}


Vector<int> Building::getSize() const {
	int x = SIZES[(int)this->type].x;

	if (x >= 0)
		return {x, SIZES[(int)this->type].y};

	switch (this->type) {
	case BuildingType::OIL_FIELD:
		return {this->oilField.size, this->oilField.size};

	case BuildingType::CONSTRUCTION:
		return this->construction.goal->getSize();

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

	case BuildingType::CONSTRUCTION:
		*ptr++ = {0, 0};
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

	case BuildingType::CONSTRUCTION:
		*ptr++ = {0, 0};
		break;


	}

	return (int)(ptr-list);
}



struct Param {
	float a;
	float b;
};
static constexpr Param solveConstSettings(float low_time, float huge_time) {
	float y1 = low_time * 3600.0f;
    float y2 = huge_time * 3600.0f;

    float b = (2.0f * y2 - 4.0f * y1) / (16.0f * y1 - 4.0f * y2);
    float a = y1 / (2.0f + 4.0f * b);

    return {a, b};
}

static const auto SOLVE_SETTINGS = solveConstSettings(2, 12);


int Building::getConstructionCost() const {
	switch (this->type) {
	case BuildingType::HOME:
	{
		float c = (float)this->home.capacity;
		float cost = SOLVE_SETTINGS.a * (c + SOLVE_SETTINGS.b * c*c);
		return (int)ceilf(cost);
	}
	case BuildingType::OIL_FIELD:
	{
		return 2'000'000'000;
	}
	case BuildingType::PLANTATION:
	{
		return 2'000'000'000;;
	}
	case BuildingType::GROCERY:
	{
		return 2'000'000'000;;
	}
	case BuildingType::CONSTRUCTION:
	{
		return 0;
	}


	}
	return 0;
}



int Building::enter(Character* c) {
	switch (this->type) {
	case BuildingType::HOME:
	{
		return 0;
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
		float max = this->grocery.cashierEfficiency *
			(float)this->grocery.cashiers;

		if ((float)this->grocery.clients >= max)
			return -1;

		return 0;
	}

	case BuildingType::CONSTRUCTION:
		return 0;

	}


	return -1;
}

void Building::leave(int position) {
	switch (this->type) {
	case BuildingType::HOME:
	{
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

	case BuildingType::CONSTRUCTION:
	{
		break;
	}
	}

}

bool Building::isFull() const {
	switch (this->type) {
	case BuildingType::HOME:
		return false;

	case BuildingType::OIL_FIELD:
		return this->oilField.leftEmployees == 0;

	case BuildingType::PLANTATION:
		return false;

	case BuildingType::GROCERY:
	{
		float max = this->grocery.cashierEfficiency * 
			(float)this->grocery.cashiers;
		return ((float)this->grocery.clients < max);
	}

	case BuildingType::CONSTRUCTION:
		return false;

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
		auto job = this->oilField.job;
		auto result = (uint32_t*)malloc(sizeof(uint32_t)*(COUNT+2));

		result[0] = COUNT; // length (as uint32_t)
		result[1] = (uint32_t)PanelId::BUILDING_OIL_FIELD;
		result[2] = flt(this->oilField.crude);
		result[3] = flt(this->oilField.refined);
		result[4] = job->startTime.hour;
		result[5] = job->finishTime.hour;
		result[6] = flt(job->salaryPerLiter);
		result[7] = flt(job->pricePerLiter);
		result[8] = job->employeesCounters.raffiners.current;
		result[9] = job->employeesCounters.raffiners.goal;
		return result;
	}

	case BuildingType::PLANTATION:
	{
		static constexpr int COUNT = 1;
		auto result = (uint32_t*)malloc(sizeof(uint32_t)*(COUNT+2));

		/// TODO: that
		result[0] = COUNT;
		result[1] = (uint32_t)-1;

		return result;
	}

	case BuildingType::GROCERY:
	{
		static constexpr int COUNT = 5;
		auto job = this->grocery.job;
		auto result = (uint32_t*)malloc(sizeof(uint32_t)*(COUNT+2));

		result[0] = COUNT; // length (as uint32_t)
		result[1] = (uint32_t)PanelId::BUILDING_GROCERY;
		result[2] = this->grocery.clients;
		result[3] = this->grocery.cashiers;
		result[4] = flt(this->grocery.stock);
		result[5] = flt(this->grocery.cashierEfficiency);
		result[6] = flt(job->salaryPerHour);
		return result;
	}

	case BuildingType::CONSTRUCTION:
	{
		static constexpr int COUNT = 5;
		auto job = this->construction.job;
		auto result = (uint32_t*)malloc(sizeof(uint32_t)*(COUNT+2));

		printf("called\n");
		result[0] = COUNT; // length (as uint32_t)
		result[1] = (uint32_t)PanelId::BUILDING_CONSTRUCTION;
		result[2] = (uint32_t)this->construction.goal->type;
		result[3] = this->construction.completion;
		result[4] = this->construction.total;
		result[5] = job->employeesCounters.workers.current;
		result[6] = job->employeesCounters.workers.goal;

		return result;
	}


	}


	#undef flt
	return nullptr;
}

bool Building::setPanelData(const uint32_t* data, Game& game) {
	#define flt(x) ((*(uint32_t*)&x))

	switch (this->type) {
	case BuildingType::HOME:
	{
		this->home.rent = data[0];

		// Destroy building
		if (data[1]) {
			return true;
		}
		break;
	}

	case BuildingType::OIL_FIELD:
	{
		auto job = this->oilField.job;

		job->startTime.hour = (short)(data[0]%24);
		job->finishTime.hour = (short)(data[1]%24);
		flt(job->salaryPerLiter) = data[2];
		flt(job->pricePerLiter) = data[3];
		job->employeesCounters.raffiners.goal = data[4];
		break;
	}

	case BuildingType::PLANTATION:
	{
		// No data to edit
		break;
	}

	case BuildingType::GROCERY:
	{
		auto job = this->grocery.job;

		float efficiency;
		float salary;

		flt(efficiency) = data[0];
		flt(salary) = data[1];

		if (salary != job->salaryPerHour) {
			job->setSalary(game, this, salary);
		} else if (efficiency != this->grocery.cashierEfficiency) {
			job->setEfficiency(game, this, efficiency);
		}

		break;
	}

	case BuildingType::CONSTRUCTION:
	{
		auto job = this->construction.job;
		job->employeesCounters.workers.goal = data[0];
		break;
	}

	}

	#undef flt

	return false;
}

Job* Building::getJob() {
	switch (this->type) {
	case BuildingType::HOME:
		return nullptr;
	
	case BuildingType::OIL_FIELD:
		return this->oilField.job;

	case BuildingType::PLANTATION:
		return this->plantation.job;

	case BuildingType::GROCERY:
		return this->grocery.job;

	case BuildingType::CONSTRUCTION:
		return this->construction.job;
	}

	return nullptr;
}



void Building::destroy(Game& game) {
	switch (this->type) {
	case BuildingType::HOME:
	{
		for (int i = 0; i < this->home.capacity; i++) {
			auto c = this->home.characters[i];
			if (c) {
				c->kickFromHouse();
			}
		}

		delete[] this->home.characters;
		break;
	}

	case BuildingType::OIL_FIELD:
	{
		break;
	}

	case BuildingType::PLANTATION:
	{
		break;
	}

	case BuildingType::GROCERY:
	{
		break;
	}

	case BuildingType::CONSTRUCTION:
	{
		auto goal = this->construction.goal;
		if (goal) {
			goal->destroy(game);
			delete goal;
		}
		break;
	}
	}

	Job* job = this->getJob();
	if (job) {
		job->destroy(game, this->owner);
	}

	
	#if TESTING_SERV
	this->hasBeenDestroyed = true;
	#endif

}


bool Building::home_addCharacter(Character* c) {
	#if TESTING_SERV
	if (this->type != BuildingType::HOME) {
		throw std::runtime_error{"HOME type was expected"};
	}
	#endif


	if (this->home.left == 0)
		return false;

	for (int i = 0; i < this->home.capacity; i++) {
		if (this->home.characters[i] == nullptr) {
			this->home.left--;
			this->home.characters[i] = c;
			return true;
		}
	}

	this->home.left = 0;
	return false;
}

bool Building::home_removeCharacter(Character* c) {
	#if TESTING_SERV
	if (this->type != BuildingType::HOME) {
		throw std::runtime_error{"HOME type was expected"};
	}
	#endif

	for (int i = 0; i < this->home.capacity; i++) {
		if (this->home.characters[i] == c) {
			this->home.left--;
			this->home.characters[i] = nullptr;
			return true;
		}
	}

	return false;
}









Building::~Building() {
	#if TESTING_SERV
	if (!this->hasBeenDestroyed) {
		printWarn("Building not destroyed");
	}
	#endif
}
