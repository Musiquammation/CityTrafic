#include "Calendar.hpp"

// constexpr float DAYS_PER_HOUR = 365.0f/(24*7);
constexpr float DAYS_PER_HOUR = 365.0f/(12);

// Number of days per month (-1 = February, handled separately)
const int Calendar::MONTH_DAYS[12] = {
	31, // January
	-1, // February (special case: leap years)
	31, // March
	30, // April
	31, // May
	30, // June
	31, // July
	31, // August
	30, // September
	31, // October
	30, // November
	31  // December
};

const char* const Calendar::WEEK_DAYS[7] = {
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
	"Sunday"
};

const int Calendar::WORKING_DAYS[6] = {
	0, 1, 2, 3, 4, -1
};
const int Calendar::DECALED_DAYS[6] = {
	1, 2, 3, 4, 5, -1
};
const int Calendar::EXTENDED_DAYS[7] = {
	0, 1, 2, 3, 4, 5, -1
};

const int Calendar::EVERY_DAYS[8] = {
	0, 1, 2, 3, 4, 5, 6, -1
};


void Calendar::move() {
	// Get rest
	static constexpr float MINUTES_PER_FRAME =
		DAYS_PER_HOUR * (1440.0f / (3600.0f * 60.0f));

	rest += MINUTES_PER_FRAME;

	// One minute
	while (rest >= 1.0f) {
		rest -= 1.0f;
		minute++;
		indicator++;

		if (minute < 60) continue;
		minute = 0;
		hour++;

		if (hour < 24) continue;
		hour = 0;
		day++;
		weekDay = (weekDay + 1) % 7;
		totalDay++;
		dayIndicator = indicator;

		// Months
		int daysInMonth = MONTH_DAYS[month];
		if (daysInMonth == -1) {
			// February
			daysInMonth = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 29 : 28;
		}

		if (day < daysInMonth) continue;
		day = 0;
		month++;

		if (month < 12) continue;
		month = 0;
		year++;
	}
}

calendar_t Calendar::getTime(calendar_t day, instant_t instant) {
	return 
		+ day * (24*60)
		+ instant.hour * 60 
		+ instant.minute;
}

#include <stdio.h>
calendar_t Calendar::getFutureInstant(calendar_t base, instant_t time, const int* days) {
	calendar_t base_totalDay = base / (60*24);
	calendar_t base_weekDay = base_totalDay % 7;


	for (calendar_t offset = 0; offset < 7; offset++) {
		int targetWeekDay = int((base_weekDay + offset) % 7);

		bool isAllowed;
		if (days) {
			isAllowed = false;
			for (int i = 0; days[i] != -1; i++) {
				if (days[i] == targetWeekDay) {
					isAllowed = true;
					break;
				}
			}
		} else {
			isAllowed = true;
		}

		if (isAllowed) {
			calendar_t potentialIndicator = getTime(
				base_totalDay + offset,
				time
			);

			if (offset == 0) {
				if (potentialIndicator > base) {
					printf("date[direct] %ld\n", potentialIndicator);
					return potentialIndicator;
				}
			} else {
				printf("date[relatd] %ld\n", potentialIndicator);
				return potentialIndicator;
			}
		}
	}

	return NOTIME;
}

void Calendar::updateIndicator(calendar_t indicator) {
	// Update the main absolute indicator
	this->indicator = indicator;

	// Temporary value for decomposition
	calendar_t remainingMinutes = indicator;

	// Reset basic fields
	minute = (int)(remainingMinutes % 60);
	calendar_t totalHours = remainingMinutes / 60;

	hour = (int)(totalHours % 24);
	calendar_t totalDays = totalHours / 24;

	// Update daily trackers
	this->totalDay = totalDays;
	this->dayIndicator = totalDays * 24 * 60;

	// Assuming the calendar starts on Monday (as per WEEK_DAYS array order)
	this->weekDay = (int)(totalDays % 7);

	// Decompose days into Years, Months, and Days
	year = 0; // Or your starting base year (e.g., 2000)
	month = 0;
	day = 0;

	// Calculate Years
	while (true) {
		bool isLeap = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
		calendar_t daysInYear = isLeap ? 366 : 365;

		if (totalDays < daysInYear) break;

		totalDays -= daysInYear;
		year++;
	}

	// Calculate Months
	while (true) {
		int daysInMonth = MONTH_DAYS[month];
		if (daysInMonth == -1) {
			// February leap year check
			bool isLeap = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
			daysInMonth = isLeap ? 29 : 28;
		}

		if (totalDays < (calendar_t)daysInMonth) break;

		totalDays -= (calendar_t)daysInMonth;
		month++;

		if (month >= 12) {
			month = 0;
			year++;
		}
	}

	// Remaining days
	this->day = (int)totalDays;

	// Clear the fractional rest to avoid synchronization jumps
	this->rest = 0.0f;
}