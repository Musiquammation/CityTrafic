#include "EmployeesCounter.hpp"

bool EmployeesCounter::canHire() const {
	return this->current < this->goal;
}

bool EmployeesCounter::hire() {
	if (this->current >= this->goal)
		return false;

	this->current++;
	return true;
}
