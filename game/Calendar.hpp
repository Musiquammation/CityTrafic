#pragma once

#include "calendar_t.hpp"

class Calendar {
	static const int MONTH_DAYS[12];

	float rest = 0;
	calendar_t dayIndicator = 0;

public:
	static const char* const WEEK_DAYS[7];
	static const int WORKING_DAYS[6]; // Monday-Friday
	static const int DECALED_DAYS[6]; // Tuesday-Saturday
	static const int EXTENDED_DAYS[7]; // Monday-Saturday

	static constexpr calendar_t NOTIME = (calendar_t)(-1);
	static calendar_t getTime(calendar_t day, instant_t time);

	int minute = 0;
	int hour = 0;
	int weekDay = 0; // Monday(0), Tuesday(1), etc...
	int day = 0;
	int month = 0;
	int year = 0;
	calendar_t indicator = 0;
	calendar_t totalDay;
	calendar_t totalMonth;

	void move();


	calendar_t getFutureInstant(
		instant_t time,
		const int* days
	) const;
	
};