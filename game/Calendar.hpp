#pragma once

#include "calendar_t.hpp"

class Calendar {
	static constexpr float DAYS_PER_HOUR = 365.0f/(24*7);
	static const int MONTH_DAYS[12];

	float rest = 0;
	calendar_t dayIndicator = 0;

public:
	static const char* const WEEK_DAYS[7];
	static constexpr calendar_t NOTIME = (calendar_t)(-1);

	int minute = 0;
	int hour = 0;
	int weekDay = 0; // Monday(0), Tuesday(1), etc...
	int day = 0;
	int month = 0;
	int year = 0;
	calendar_t indicator = 0;
	long long totalDay;

	void move();

	// Get time relative to today 0am
	calendar_t getTime(int days, int hour, int mn);


};