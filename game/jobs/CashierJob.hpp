#pragma once

#include "EmployeesCounter.hpp"

#include "../Job.hpp"
#include "../calendar_t.hpp"

#include <map>


class CashierJob: public Job {
	struct WorkerData {
		float toPay;
		bool willWork;
		calendar_t meeting;
		calendar_t entryHour;
	};

	
	Vector<int> location;
	std::map<Character*, WorkerData> workers;
	

public:
	CashierJob(
		Vector<int> location
	);

	~CashierJob();

	static float evalSalary(float efficiency);
	static float evalEfficiency(float salary);


	struct {
		EmployeesCounter cashiers;
	} employeesCounters;

	float salaryPerHour = 0.0f;

	instant_t startTime = {11,0};
	instant_t finishTime = {18,0};




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

	void setSalary(Game& game, float salary);
	void setEfficiency(Game& game,float efficiency);
};
