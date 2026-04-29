#include "Calendar.hpp"

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


void Calendar::move() {
    // Get rest
    static constexpr float MINUTES_PER_FRAME =
        (float)DAYS_PER_HOUR * (1440.0f / (3600.0f * 60.0f));

    rest += MINUTES_PER_FRAME;

    // One minute
    while (rest >= 1.0f) {
        rest -= 1.0f;
        minute++;

        if (minute < 60) continue;
        minute = 0;
        hour++;

        if (hour < 24) continue;
        hour = 0;
        day++;
        weekDay = (weekDay + 1) % 7;
        totalDay++;

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