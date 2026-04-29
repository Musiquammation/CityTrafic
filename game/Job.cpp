#include "Job.hpp"

#include "Character.hpp"

void Job::earn(int money) {
	this->money += money;
}

int Job::pay(int money) {
	if (this->money >= money) {
		this->money -= money;
		return money;
	}

	int given = this->money;
	this->money = 0;
	return given;
}

void Job::fireEveryone() {
	this->forAllWorkers([](Character* c) {
		c->leaveJob(); 
	});
}