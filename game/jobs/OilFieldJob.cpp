#include "OilFieldJob.hpp"

#include "../Game.hpp"
#include "../Map.hpp"
#include "../Calendar.hpp"
#include "../Building.hpp"

OilFieldJob::OilFieldJob(
	Vector<int> location,
	float salaryPerUnit
):
	location(location),
	salaryPerUnit(salaryPerUnit)
{}

OilFieldJob::~OilFieldJob() {
	this->fireEveryone();
}

calendar_t OilFieldJob::getNextHour(
	worker_t worker,
	const Calendar& calendar
) {
	return Calendar::NOTIME;
}

int OilFieldJob::getSalary(
	worker_t worker,
	const Calendar& calendar
) {
	return 0;
}


Vector<int> OilFieldJob::getEmployeeSite(
	worker_t worker,
	const Calendar& calendar
) {
	return this->location;
}

void OilFieldJob::work(
	worker_t worker,
	const Calendar& calendar,
	Game& game
) {
	auto building = game.getBuilding(
		this->location.x, this->location.y).building;

	if (!building) {
		throw std::runtime_error{"Missing work building"};
	}

	if (building->type != BuildingType::OIL_FIELD)
		throw std::runtime_error{"A OIL_FIELD building was expected"};

	float c = building->oilField.crude;
	float n = c * building->oilField.factor;

	float d = c-n;
	auto it = this->workers.find((Character*)worker);

	if (it != this->workers.end()) {
		it->second.toPay += d * this->salaryPerUnit;
	}


	building->oilField.crude = n;
}


bool OilFieldJob::hire(Character* worker) {
	// Check if worker is already in workers
	if (this->workers.find(worker) != this->workers.end()) {
		return false; // Worker is already hired
	}
	// Add worker to the list with initial data
	this->workers[worker] = WorkerData{0.0f};
	return true;
}

void OilFieldJob::fire(Character* worker) {
	// Find and remove the worker from the list
	this->workers.erase(worker);
}


void OilFieldJob::forAllWorkers(
	std::function<void(Character*)> fn
) {
	for (auto i: this->workers) {
		fn(i.first);
	}
}

