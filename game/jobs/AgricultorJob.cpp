#include "AgricultorJob.hpp"

#include "../JobOffer.hpp"
#include "../Game.hpp"
#include "../Map.hpp"
#include "../Calendar.hpp"
#include "../Building.hpp"

#include <math.h>

AgricultorJob::AgricultorJob(
	Vector<int> location,
	float salaryPerSeed,
	float pricePerSeed
):
	location(location),
	salaryPerSeed(salaryPerSeed),
	pricePerSeed(pricePerSeed)
{}

AgricultorJob::~AgricultorJob() {
	this->fireEveryone();
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
	const Calendar& calendar
) {
	return this->location;
}

void AgricultorJob::work(
	worker_t worker,
	Game& game
) {
	auto building = game.getBuilding(
		this->location.x, this->location.y).building;

	if (!building) {
		throw std::runtime_error{"Missing work building"};
	}

	if (building->type != BuildingType::PLANTATION)
		throw std::runtime_error{"A PLANTATION building was expected"};


    building->plantation.couldown--;
    if (building->plantation.couldown <= 0) {
        building->plantation.couldown += building->plantation.delay;
        building->plantation.stock += 1.0f;
    }

    auto it = this->workers.find((Character*)worker);

	if (it != this->workers.end()) {
		it->second.toPay += this->salaryPerSeed /
            (float)building->plantation.delay;
	}

}

void AgricultorJob::onEnter(
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

	it->second.willWork = false;
}

void AgricultorJob::onLeave(
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


	it->second.willWork = true;

}

bool AgricultorJob::hire(
	Character* worker,
	const JobOffer& offer,
	const Calendar& calendar
) {
	// Check if worker is already in workers
	if (this->workers.find(worker) != this->workers.end()) {
		return false; // Worker is already hired
	}


	switch (offer.type) {
	case JobOfferType::AGRICULTOR:
		if (!this->employeesCounters.agricultors.canHire()) {return false;}
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



float AgricultorJob::getPricePerSeed(const Game& game) const {
	auto building = game.getBuilding(
		this->location.x, this->location.y).building;

	if (!building) {
		throw std::runtime_error{"Missing work building"};
	}

	if (building->type != BuildingType::OIL_FIELD)
		throw std::runtime_error{"A OIL_FIELD building was expected"};

	if (building->oilField.refined >= 1.0f)
		return this->pricePerSeed;

	return 1e20f; // A lot : cannot be bought
}

float AgricultorJob::buy(Game& game, int money) {
	float seeds = (float)money / this->pricePerSeed;

	auto building = game.getBuilding(
		this->location.x, this->location.y).building;

	if (!building) {
		throw std::runtime_error{"Missing work building"};
	}

	if (building->type != BuildingType::OIL_FIELD)
		throw std::runtime_error{"A OIL_FIELD building was expected"};

	// Enough refined oil to buy
	if (building->oilField.refined >= seeds) {
		building->oilField.refined -= seeds;
		this->give(money);
		return seeds;
	}

	return 0;
}