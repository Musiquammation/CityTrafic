#pragma once

struct EmployeesCounter {
	int current = 0;
	int goal = 0;

	bool canHire() const;
};