#include "Calendar.hpp"

#include <stdio.h>

// constexpr float DAYS_PER_HOUR = 365.0f/(24*7);
constexpr float DAYS_PER_HOUR = 365.0f/4;

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


calendar_t Calendar::getFutureInstant(instant_t time, const int* days) const {
    calendar_t targetToday = getTime(totalDay, time);

    for (int offset = 0; offset < 7; offset++) {
        int targetWeekDay = (this->weekDay + offset) % 7;
        
        bool isAllowed = false;
        for (int i = 0; days[i] != -1; i++) {
            if (days[i] == targetWeekDay) {
                isAllowed = true;
                break;
            }
        }

        if (isAllowed) {
            calendar_t potentialIndicator = getTime(totalDay + offset, time);
            
            if (offset == 0) {
                if (potentialIndicator > this->indicator) {
                    return potentialIndicator;
                }
            } else {
                return potentialIndicator;
            }
        }
    }

    return NOTIME;
}