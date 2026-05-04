#include "EmployeesCounter.hpp"

bool EmployeesCounter::canHire() const {
	return this->current < this->goal;
}
