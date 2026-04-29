#pragma once

#include "Vector.hpp"
#include "calendar_t.hpp"
#include "declarations.hpp"

#include <functional>

class Job {
	int money;
	int prevPayMonth = -1;
	
protected:
	using worker_t = const Character*;
	void fireEveryone();

	
public:
	static constexpr int NO_LOCATION = INT32_MIN;

	void earn(int money);
	int pay(int money);

	virtual calendar_t getNextEnterHour(
		worker_t worker,
		const Calendar& calendar
	) = 0;

	virtual calendar_t getNextLeaveHour(
		worker_t worker,
		const Calendar& calendar
	) = 0;

	virtual int getSalary(
		worker_t worker,
		const Calendar& calendar
	) = 0;

	virtual Vector<int> getEmployeeSite(
		worker_t worker,
		const Calendar& calendar
	) = 0;

	virtual void work(
		worker_t worker,
		Game& game
	) = 0;


	virtual void onEnter(
		worker_t worker,
		const Calendar& calendar
	) = 0;

	virtual void onLeave(
		worker_t worker,
		const Calendar& calendar
	) = 0;


	virtual bool hire(Character* worker, const Calendar& calendar) = 0;

	virtual void fire(Character* worker) = 0;

	virtual void forAllWorkers(std::function<void(Character*)>) = 0;


	virtual ~Job() = default;

};