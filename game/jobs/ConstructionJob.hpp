#pragma once

#include "EmployeesCounter.hpp"

#include "../Job.hpp"
#include "../calendar_t.hpp"

#include <map>


class ConstructionJob: public Job {
	struct WorkerData {
		float toPay;
		bool willWork;
		calendar_t meeting;
	};

	
	std::map<Character*, WorkerData> workers;
	
	friend struct jobSerializator;

public:
	ConstructionJob(
		float salaryPerUnit
	);

	~ConstructionJob();


	struct {
		EmployeesCounter workers;
	} employeesCounters;

	instant_t startTime = {7,0};
	instant_t finishTime = {15,0};

	float salaryPerUnit;



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
		Vector<int> loc,
		Building* building,
		const Calendar& calendar
	) override;


	void work(
		worker_t worker,
		Vector<int> loc,
		Building* building,
		Game& game
	) override;

	void onEnter(
		worker_t worker,
		Building* building,
		const Calendar& calendar
	) override;

	void onLeave(
		worker_t worker,
		Building* building,
		const Calendar& calendar
	) override;
	
	bool hire(
		Character* worker,
		Building* building,
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

};
