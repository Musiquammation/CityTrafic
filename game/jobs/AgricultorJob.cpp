#include "AgricultorJob.hpp"

#include "../JobOffer.hpp"
#include "../Game.hpp"
#include "../Map.hpp"
#include "../Calendar.hpp"
#include "../Building.hpp"

#include <cmath>
#include <stdexcept>

AgricultorJob::AgricultorJob(
	float salaryPerSeed,
	float pricePerSeed
):
	salaryPerSeed(salaryPerSeed),
	pricePerSeed(pricePerSeed)
{}

AgricultorJob::~AgricultorJob() {
	
}

calendar_t AgricultorJob::getNextEnterHour(
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

calendar_t AgricultorJob::getNextLeaveHour(
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

int AgricultorJob::getSalary(
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


Vector<int> AgricultorJob::getEmployeeSite(
	worker_t worker,
	Vector<int> loc,
	Building* building,
	const Calendar& calendar
) {
	return loc;
}

bool AgricultorJob::work(
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

	if (workBuilding->type != BuildingType::PLANTATION)
		throw std::runtime_error{"A PLANTATION building was expected"};

	workBuilding->plantation.cooldown--;
	if (workBuilding->plantation.cooldown <= 0) {
		workBuilding->plantation.cooldown += workBuilding->plantation.delay;
		workBuilding->plantation.stock += 1.0f;
	}

	auto it = this->workers.find((Character*)worker);

	if (it != this->workers.end()) {
		it->second.toPay += this->salaryPerSeed /
			(float)workBuilding->plantation.delay;
	}

	return false;
}

void AgricultorJob::onEnter(
	worker_t worker,
	Building* building,
	Game &game
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return;
	
	it->second.meeting = game.getCalendar().getFutureInstant(
		this->finishTime,
		Calendar::WORKING_DAYS
	);

	it->second.willWork = false;
}

void AgricultorJob::onLeave(
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

bool AgricultorJob::hire(
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
	case JobOfferType::AGRICULTOR:
		if (!this->employeesCounters.agricultors.hire()) {return false;}
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

void AgricultorJob::fire(Character* worker) {
	// Find and remove the worker from the list
	this->workers.erase(worker);
}


void AgricultorJob::forAllWorkers(
	std::function<void(Character*)> fn
) {
	for (auto i: this->workers) {
		fn(i.first);
	}
}

bool AgricultorJob::searchJobOffer(
	const Character* candidate,
	JobOffer& offer
) const {
	if (this->employeesCounters.agricultors.canHire()) {
		offer.type = JobOfferType::AGRICULTOR;
		offer.salaryEstimation = int(this->salaryPerSeed * 200);
		return true;
	}

	return false;
}




uint32_t* AgricultorJob::getPanelData() {
	throw std::runtime_error{"TODO"};
}


void AgricultorJob::setPanelData(const uint32_t* data) {

}



float AgricultorJob::getPricePerSeed(const Building* building) const {
	if (building->oilField.refined >= 1.0f)
		return this->pricePerSeed;

	return 1e20f; // A lot : cannot be bought
}

float AgricultorJob::buy(Building* building, int money) {
	float seeds = (float)money / this->pricePerSeed;


	// Enough refined oil to buy
	if (building->oilField.refined >= seeds) {
		building->oilField.refined -= seeds;
		this->give(money);
		return seeds;
	}

	return 0;
}