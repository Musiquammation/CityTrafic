#pragma once

#include "EmployeesCounter.hpp"

#include "../Job.hpp"
#include "../calendar_t.hpp"

#include <map>


class OilFieldJob: public Job {
	struct WorkerData {
		float toPay;
		bool willWork;
		calendar_t meeting;
	};

	
	instant_t startTime = {7,0};
	instant_t finishTime = {15,0};
	Vector<int> location;
	float salaryPerLiter;
	float pricePerLiter;
	std::map<Character*, WorkerData> workers;
	

public:
	OilFieldJob(
		Vector<int> location,
		float salaryPerLiter,
		float pricePerLiter
	);

	~OilFieldJob();


	struct {
		EmployeesCounter raffiners;
	} employeesCounters;

	calendar_t getNextEnterHour(
		worker_t worker,
		const Calendar& calendar
	) override;

	calendar_t getNextLeaveHour(
		worker_t worker,
		const Calendar& calendar
	) override;

	int getSalary(
		worker_t worker,
		const Calendar& calendar
	) override;

	Vector<int> getEmployeeSite(
		worker_t worker,
		const Calendar& calendar
	) override;

	void work(
		worker_t worker,
		Game& game
	) override;

	void onEnter(
		worker_t worker,
		const Calendar& calendar
	) override;

	void onLeave(
		worker_t worker,
		const Calendar& calendar
	) override;
	
	bool hire(
		Character* worker,
		const JobOffer& offer,
		const Calendar& calendar
	) override;

	void fire(Character* worker) override;

	void forAllWorkers(
		std::function<void(Character*)>
	) override;

	bool searchJobOffer(const Character* candidate,
		JobOffer& offer) const override;

	uint32_t* getPanelData() override;
	void setPanelData(const uint32_t* data) override;

	float getPricePerLiter(const Game& game) const;
	float buy(Game& game, int money);

};
