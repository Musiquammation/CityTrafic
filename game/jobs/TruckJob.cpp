#include "TruckJob.hpp"

#include "../JobOffer.hpp"
#include "../Game.hpp"
#include "../Calendar.hpp"
#include "../Building.hpp"

#include <math.h>
#include <stdexcept>

#include "../ActionExecutor.hpp"
#include "../Character.hpp"
#include "../actions/action_truck.hpp"


enum {
	DECALAGE_RESET = 1000
};

TruckJob::WorkerData::~WorkerData() {
	delete this->executor;
}

TruckJob::TruckJob() {

}

TruckJob::~TruckJob() {

}





calendar_t TruckJob::getNextEnterHour(
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

calendar_t TruckJob::getNextLeaveHour(
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

int TruckJob::getSalary(
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


Vector<int> TruckJob::getEmployeeSite(
	worker_t worker,
	Vector<int> loc,
	Building* building,
	const Calendar& calendar
) {
	return loc;
}

static Building* getBuilding(Game& game, Vector<int> loc) {
	auto building = game.getBuilding(
		loc.x, loc.y).building;

	if (!building) {
		throw std::runtime_error{"Missing work building"};
	}

	if (building->type != BuildingType::WAREHOUSE)
		throw std::runtime_error{"A WAREHOUSE building was expected"};

	return building;
}

bool TruckJob::work(
	worker_t worker,
	Vector<int> loc,
	Building *building,
	Game &game
) {
	Building* workBuilding = building;
	if (!workBuilding) {
		workBuilding = getBuilding(game, loc);
	}

	if (auto it = this->workers.find((Character*)worker); it != this->workers.end()) {
		auto& data = it->second;
		if (data.executor->run(game)) {
			// Finished
			return true;
		}
	}

	return false;
}

void TruckJob::onEnter(
	worker_t worker,
	Building* building,
	Game &game
) {
	auto it = this->workers.find((Character*)worker);
	if (it == this->workers.end())
		return;

	auto& data = it->second;
	data.meeting = game.getCalendar().getFutureInstant(
		this->finishTime,
		Calendar::WORKING_DAYS
	);

	data.entryHour = game.getCalendar().indicator;
	data.willWork = false;


	if (data.executor) {
		throw std::runtime_error{"Executor must be empty"};
	}



	// Get targets
	const auto& map = game.getMap();
	std::vector<Vector<int>> targetVect;
	for (auto it = map.buildings_begin(); it != map.buildings_end(); ++it) {
		auto r = it->second->fillTruckImports(
			map,
			it->first,
			targetVect
		);

		if (r) {
			targetVect.push_back(it->first);
		}
	}

	int length = (int)targetVect.size();
	auto targets = new Vector<int>[length];

	int decalage = this->importsDecalage;
	for (int i = 0; i < length; ++i) {
		targets[i] = targetVect[(i + decalage) % length];
	}

	data.executor = actionNodes::truck::createExecutor(
		(Character*)worker,
		targets,
		length
	);


	// Move decalages
	this->importsDecalage++;
	if (this->importsDecalage >= DECALAGE_RESET) {
		this->importsDecalage -= DECALAGE_RESET;
	}
}

void TruckJob::onLeave(
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


	int elapsed = int(calendar.indicator - it->second.entryHour);
	it->second.willWork = true;
	it->second.toPay += (float)elapsed * (this->salaryPerHour/60);

	delete it->second.executor;
	it->second.executor = nullptr;
}

bool TruckJob::hire(
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
	case JobOfferType::TRUCK :
		if (!this->employeesCounters.truckers.hire()) {return false;}
		break;

	default:
		return false;
	}

	// Add worker to the list with initial data
	this->workers[worker] = WorkerData{
		.toPay = 0.0f,
		.willWork = true,
		.meeting = calendar.getFutureInstant(
			this->startTime,
			Calendar::WORKING_DAYS
		),
		.entryHour = Calendar::NOTIME,
		.executor = nullptr
	};


	return true;
}

void TruckJob::fire(Character* worker) {
	// Find and remove the worker from the list
	this->workers.erase(worker);
}


void TruckJob::forAllWorkers(
	std::function<void(Character*)> fn
) {
	for (auto i: this->workers) {
		fn(i.first);
	}
}

bool TruckJob::searchJobOffer(
	const Character* candidate,
	JobOffer& offer
) const {
	if (this->employeesCounters.truckers.canHire()) {
		offer.type = JobOfferType::TRUCK;
		offer.salaryEstimation = int(this->salaryPerHour * 200);
		return true;
	}

	return false;
}




uint32_t* TruckJob::getPanelData() {
	throw std::runtime_error{"TODO"};
}


void TruckJob::setPanelData(const uint32_t* data) {
	
}


