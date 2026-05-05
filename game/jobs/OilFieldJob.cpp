#include "OilFieldJob.hpp"

#include "../JobOffer.hpp"
#include "../Game.hpp"
#include "../Map.hpp"
#include "../Calendar.hpp"
#include "../Building.hpp"

#include <math.h>

OilFieldJob::OilFieldJob(
	float salaryPerLiter,
	float pricePerLiter
):
	salaryPerLiter(salaryPerLiter),
	pricePerLiter(pricePerLiter)
{}

OilFieldJob::~OilFieldJob() {
	
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
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return 0;

	auto& data = it->second;
	float f = std::floor(data.toPay);
	data.toPay -= f;
	return (int)f;
}


Vector<int> OilFieldJob::getEmployeeSite(
	worker_t worker,
	Vector<int> loc,
	Building* building,
	const Calendar& calendar
) {
	return loc;
}

void OilFieldJob::work(
	worker_t worker,
	Vector<int> loc,
	Building* building,
	Game& game
) {
	Building* workBuilding = building;
	if (!workBuilding) {
		workBuilding = game.getBuilding(loc.x, loc.y).building;
	}

	if (!workBuilding) {
		throw std::runtime_error{"Missing work building"};
	}

	if (workBuilding->type != BuildingType::OIL_FIELD)
		throw std::runtime_error{"A OIL_FIELD building was expected"};

	float c = workBuilding->oilField.crude;
	float n = c * workBuilding->oilField.factor;

	float d = c-n;
	auto it = this->workers.find((Character*)worker);

	if (it != this->workers.end()) {
		it->second.toPay += d * this->salaryPerLiter;
	}


	building->oilField.refined += d;
	building->oilField.crude = n;
}

void OilFieldJob::onEnter(
	worker_t worker,
	Building* building,
	const Calendar& calendar
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return;
	
	it->second.meeting = calendar.getFutureInstant(
		this->finishTime,
		Calendar::WORKING_DAYS
	);

	it->second.willWork = false;
}

void OilFieldJob::onLeave(
	worker_t worker,
	Building* building,
	const Calendar& calendar
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return;
	
	it->second.meeting = calendar.getFutureInstant(
		this->startTime,
		Calendar::WORKING_DAYS
	);


	it->second.willWork = true;

}

bool OilFieldJob::hire(
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
	case JobOfferType::OIL_RAFFINER:
		if (!this->employeesCounters.raffiners.canHire()) {return false;}
		break;

	default:
		return false;
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

bool OilFieldJob::searchJobOffer(
	const Character* candidate,
	JobOffer& offer
) const {
	if (this->employeesCounters.raffiners.canHire()) {
		offer.type = JobOfferType::OIL_RAFFINER;
		offer.salaryEstimation = int(this->salaryPerLiter * 200);
		return true;
	}

	return false;
}




uint32_t* OilFieldJob::getPanelData() {
	throw std::runtime_error{"TODO"};
}


void OilFieldJob::setPanelData(const uint32_t* data) {

}



float OilFieldJob::getPricePerLiter(const Building* building) const {
	if (building->oilField.refined >= 1.0f)
		return this->pricePerLiter;

	return 1e20f; // A lot : cannot be bought
}

float OilFieldJob::buy(Building* building, int money) {
	float liters = (float)money / this->pricePerLiter;


	// Enough refined oil to buy
	if (building->oilField.refined >= liters) {
		building->oilField.refined -= liters;
		this->give(money);
		return liters;
	}

	return 0;
}