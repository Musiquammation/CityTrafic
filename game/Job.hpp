#pragma once

#include "Vector.hpp"
#include "calendar_t.hpp"
#include "declarations.hpp"

#include <functional>

class Job {
	int money = 0;
	int prevPayMonth = -1;
	
protected:
	using worker_t = const Character*;
	
public:
	static constexpr int NO_LOCATION = INT32_MIN;

	
	void give(int money);
	int pay(int money);
	int getMoney() const;
	void destroy(Game& game, int owner);
	
	
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

	/**
	 * @warning worker can be null
	 */
	virtual Vector<int> getEmployeeSite(
		worker_t worker,
		Vector<int> loc,
		Building* building,
		const Calendar& calendar
	) = 0;

	virtual bool work(
		worker_t worker,
		Vector<int> loc,
		Building *building,
		Game &game
	) = 0;


	virtual void onEnter(
		worker_t worker,
		Building* building,
		Game &game
	) = 0;

	virtual void onLeave(
		worker_t worker,
		Building* building,
		const Calendar& calendar
	) = 0;


	virtual bool hire(
		Character* worker,
		Building* building,
		const JobOffer& offer,
		const Calendar& calendar
	) = 0;

	virtual void fire(Character* worker) = 0;

	virtual void forAllWorkers(std::function<void(Character*)>) = 0;


	virtual bool searchJobOffer(const Character* candidate,
		JobOffer& offer) const = 0;

	virtual uint32_t* getPanelData() = 0;
	virtual void setPanelData(const uint32_t* data) = 0;


	virtual ~Job() = default;

};