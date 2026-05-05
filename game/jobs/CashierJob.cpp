#include "CashierJob.hpp"

#include "../JobOffer.hpp"
#include "../Game.hpp"
#include "../Map.hpp"
#include "../Calendar.hpp"
#include "../Building.hpp"

#include <math.h>

CashierJob::CashierJob(Vector<int> location):
	location(location),
	salaryPerHour(salaryPerHour)
{}

CashierJob::~CashierJob() {
	this->fireEveryone();
}

static constexpr float EFFICIENCY_COST = 6.0f;
static constexpr float EFFICIENCY_RATIO = 0.75f;
float CashierJob::evalSalary(float efficiency) {
	static constexpr float S = EFFICIENCY_COST;
	static constexpr float R = EFFICIENCY_RATIO;
	return R * (efficiency + efficiency*efficiency*
			(1.0f/(S*S)));
}

float CashierJob::evalEfficiency(float salary) {
	static constexpr float S = EFFICIENCY_COST; 
	static constexpr float R = EFFICIENCY_RATIO;

    float discriminant = S * S + (4.0f * salary) / S;
    if (discriminant < 0.0f) {
        return -1.0f; // no real solution
    }

    float sqrt_term = sqrtf(discriminant);

    // stable form of the positive root
    float e = (-S * S + S * sqrt_term) * 0.5f;

    return e;
}



calendar_t CashierJob::getNextEnterHour(
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

calendar_t CashierJob::getNextLeaveHour(
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

int CashierJob::getSalary(
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


Vector<int> CashierJob::getEmployeeSite(
	worker_t worker,
	const Calendar& calendar
) {
	return this->location;
}

static Building* getBuilding(Game& game, Vector<int> loc) {
	auto building = game.getBuilding(
		loc.x, loc.y).building;

	if (!building) {
		throw std::runtime_error{"Missing work building"};
	}

	if (building->type != BuildingType::GROCERY)
		throw std::runtime_error{"A GROCERY building was expected"};

	return building;
}

void CashierJob::work(
	worker_t worker,
	Game& game
) {
	Building* b = getBuilding(game, this->location);
}

void CashierJob::onEnter(
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

	it->second.entryHour = calendar.indicator;
	it->second.willWork = false;
}

void CashierJob::onLeave(
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


	int elapsed = int(calendar.indicator - it->second.entryHour);
	it->second.willWork = true;
	it->second.toPay += (float)elapsed * (this->salaryPerHour/60);
		

}

bool CashierJob::hire(
	Character* worker,
	const JobOffer& offer,
	const Calendar& calendar
) {
	// Check if worker is already in workers
	if (this->workers.find(worker) != this->workers.end()) {
		return false; // Worker is already hired
	}


	switch (offer.type) {
	case JobOfferType::CASHIER :
		if (!this->employeesCounters.cashiers.canHire()) {return false;}
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
			Calendar::DECALED_DAYS
		),
		.entryHour = Calendar::NOTIME
		
	};


	return true;
}

void CashierJob::fire(Character* worker) {
	// Find and remove the worker from the list
	this->workers.erase(worker);
}


void CashierJob::forAllWorkers(
	std::function<void(Character*)> fn
) {
	for (auto i: this->workers) {
		fn(i.first);
	}
}

bool CashierJob::searchJobOffer(
	const Character* candidate,
	JobOffer& offer
) const {
	if (this->employeesCounters.cashiers.canHire()) {
		offer.type = JobOfferType::CASHIER;
		offer.salaryEstimation = int(this->salaryPerHour * 200);
		return true;
	}

	return false;
}




uint32_t* CashierJob::getPanelData() {
	throw std::runtime_error{"TODO"};
}


void CashierJob::setPanelData(const uint32_t* data) {

}


void CashierJob::setSalary(Game& game, float salary) {
	this->salaryPerHour = salary;

	Building* b = getBuilding(game, this->location);
	b->grocery.cashierEfficiency = evalEfficiency(salary);
}


void CashierJob::setEfficiency(Game& game, float efficiency) {
	this->salaryPerHour = evalSalary(efficiency);

	Building* b = getBuilding(game, this->location);
	b->grocery.cashierEfficiency = efficiency;

}


