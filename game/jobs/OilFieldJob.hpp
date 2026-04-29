#pragma once

#include "../Job.hpp"
#include <map>

class OilFieldJob: public Job {
	struct WorkerData {
		float toPay;
	};

	Vector<int> location;
	float salaryPerUnit;
	std::map<Character*, WorkerData> workers;
	

public:
	OilFieldJob(
		Vector<int> location,
		float salaryPerUnit
	);

	~OilFieldJob();

	calendar_t getNextHour(
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
		const Calendar& calendar,
		Game& game
	) override;
	
	bool hire(Character* worker) override;

	void fire(Character* worker) override;

	void forAllWorkers(
		std::function<void(Character*)>
	) override;

};