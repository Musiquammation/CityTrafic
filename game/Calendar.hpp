#pragma once

class Calendar {
	static constexpr int DAYS_PER_HOUR = 6;
	static const int MONTH_DAYS[12];

	float rest = 0;

public:
	static const char* const WEEK_DAYS[7];
	
	int minute = 0;
	int hour = 0;
	int weekDay = 0; // Monday(0), Tuesday(1), etc...
	int day = 0;
	int month = 0;
	int year = 0;
	long long totalDay;

	void move();
};