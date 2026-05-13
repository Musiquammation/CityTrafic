#include "ConstructionJob.hpp"

#include "../JobOffer.hpp"
#include "../Game.hpp"
#include "../Map.hpp"
#include "../Calendar.hpp"
#include "../Building.hpp"

#include <math.h>

ConstructionJob::ConstructionJob(
	float salaryPerUnit
):
	salaryPerUnit(salaryPerUnit)
{}

ConstructionJob::~ConstructionJob() {
	
}

calendar_t ConstructionJob::getNextEnterHour(
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

calendar_t ConstructionJob::getNextLeaveHour(
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

int ConstructionJob::getSalary(
	worker_t worker,
	const Calendar& calendar
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return 0;

	auto& data = it->second;
	float f = std::floor(data.toPay);
	data.toPay -= f;
	return (int)f;
}


Vector<int> ConstructionJob::getEmployeeSite(
	worker_t worker,
	Vector<int> loc,
	Building* building,
	const Calendar& calendar
) {
	return loc;
}

bool ConstructionJob::work(
	worker_t worker,
	Vector<int> loc,
	Building *building,
	Game &game
) {
	Building* workBuilding = building;
	if (!workBuilding) {
		workBuilding = game.getBuilding(loc.x, loc.y).building;
	}

	if (!workBuilding) {
		throw std::runtime_error{"Missing work building"};
	}

	if (workBuilding->type != BuildingType::CONSTRUCTION)
		throw std::runtime_error{"A CONSTRUCTION building was expected"};

	auto it = this->workers.find((Character*)worker);

	if (it != this->workers.end()) {
		it->second.toPay += this->salaryPerUnit;
	}

	building->construction.completion++;

    if (building->construction.completion < building->construction.total)
        return false;

    /// TODO: place final building (must finish with building->consturction.goal = nullptr)

	return true;
}

void ConstructionJob::onEnter(
	worker_t worker,
	Building* building,
	Game &game
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return;
	
	auto& data = it->second;
	data.meeting = Calendar::getFutureInstant(
		data.meeting,
		this->finishTime,
		nullptr
	);

	data.willWork = false;
}

void ConstructionJob::onLeave(
	worker_t worker,
	Building* building,
	const Calendar& calendar
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return;
	
	auto& data = it->second;
	data.meeting = Calendar::getFutureInstant(
		calendar.indicator,
		this->startTime,
		Calendar::WORKING_DAYS
	);


	data.willWork = true;

}

bool ConstructionJob::hire(
	Character* worker,
	Building* building,
	const JobOffer& offer,
	const Calendar& calendar
) {
	// Check if worker is already in workers
	if (this->workers.find(worker) != this->workers.end()) {
		return false; // Worker is already hired
	}


	switch (offer.type) {
	case JobOfferType::CONSTRUCTION:
		if (!this->employeesCounters.workers.hire()) {return false;}
		break;

	default:
		return false;
	}

	// Add worker to the list with initial data
	this->workers[worker] = WorkerData{
		0.0f,
		true,
		Calendar::getFutureInstant(
			calendar.indicator,
			this->finishTime,
			Calendar::WORKING_DAYS
		)
	};


	return true;
}

void ConstructionJob::fire(Character* worker) {
	// Find and remove the worker from the list
	this->workers.erase(worker);
}


void ConstructionJob::forAllWorkers(
	std::function<void(Character*)> fn
) {
	for (auto i: this->workers) {
		fn(i.first);
	}
}

bool ConstructionJob::searchJobOffer(
	const Character* candidate,
	JobOffer& offer
) const {
	if (this->employeesCounters.workers.canHire()) {
		offer.type = JobOfferType::CONSTRUCTION;
		offer.salaryEstimation = int(this->salaryPerUnit * 200);
		return true;
	}

	return false;
}




uint32_t* ConstructionJob::getPanelData() {
	throw std::runtime_error{"TODO"};
}


void ConstructionJob::setPanelData(const uint32_t* data) {

}



