#pragma once

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
	float salaryPerUnit;
	std::map<Character*, WorkerData> workers;
	

public:
	OilFieldJob(
		Vector<int> location,
		float salaryPerUnit
	);

	~OilFieldJob();

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
	
	bool hire(Character* worker, const Calendar& calendar) override;

	void fire(Character* worker) override;

	void forAllWorkers(
		std::function<void(Character*)>
	) override;

};