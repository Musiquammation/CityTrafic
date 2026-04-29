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

calendar_t OilFieldJob::getNextEnterHour(
	worker_t worker,
	const Calendar& calendar
) {
	auto it = this->workers.find((Character*)worker);

	if (it == this->workers.end()) {
		return Calendar::NOTIME;
	}

	if (!it->second.willWork)
		return Calendar::NOTIME;

	return it->second.meeting;
}

calendar_t OilFieldJob::getNextLeaveHour(
	worker_t worker,
	const Calendar& calendar
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return Calendar::NOTIME;
	
	if (it->second.willWork)
		return Calendar::NOTIME;

	return it->second.meeting;
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

void OilFieldJob::onEnter(
	worker_t worker,
	const Calendar& calendar
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return;
	
	it->second.meeting = calendar.getFutureInstant(
		this->finishTime,
		Calendar::WORKING_DAYS
	);

	printf("willRest: %ld\n", it->second.meeting/60);
	it->second.willWork = false;
}

void OilFieldJob::onLeave(
	worker_t worker,
	const Calendar& calendar
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return;
	
	it->second.meeting = calendar.getFutureInstant(
		this->startTime,
		Calendar::WORKING_DAYS
	);

	printf("willWork: %ld\n", it->second.meeting/60);

	it->second.willWork = true;

}

bool OilFieldJob::hire(Character* worker, const Calendar& calendar) {
	// Check if worker is already in workers
	if (this->workers.find(worker) != this->workers.end()) {
		return false; // Worker is already hired
	}
	// Add worker to the list with initial data
	this->workers[worker] = WorkerData{
		0.0f,
		true,
		calendar.getFutureInstant(
			this->startTime,
			Calendar::WORKING_DAYS
		)
	};
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

