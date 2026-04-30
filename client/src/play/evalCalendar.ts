export function evalCalendar(n: bigint) {
	const weekdays = [
		"Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Saturday", "Sunday"
	];

	const months = [
		"January", "February", "March", "April",
		"May", "June", "July", "August",
		"September", "October", "November", "December"
	];

	const MONTH_DAYS = [31n, 28n, 31n, 30n, 31n, 30n, 31n, 31n, 30n, 31n, 30n, 31n];

	// --- TIME ---
	let minute = Number(n % 60n);
	let totalHours = n / 60n;

	let hour = Number(totalHours % 24n);
	let totalDays = totalHours / 24n;

	let weekDay = Number(totalDays % 7n);

	// --- YEAR (Gregorian 400-year cycle) ---
	const DAYS_400_YEARS = 146097n; // 400*365 + 97

	let cycle = totalDays / DAYS_400_YEARS;
	let year = cycle * 400n;
	totalDays -= cycle * DAYS_400_YEARS;

	for (let i = 0; i < 400; i++) {
		let isLeap =
			(year % 400n === 0n) ||
			(year % 4n === 0n && year % 100n !== 0n);

		let daysInYear = isLeap ? 366n : 365n;

		if (totalDays >= daysInYear) {
			totalDays -= daysInYear;
			year++;
		} else {
			break;
		}
	}

	// --- MONTH ---
	let isLeapYear =
		(year % 400n === 0n) ||
		(year % 4n === 0n && year % 100n !== 0n);

	let month = 0;

	for (let i = 0; i < 12; i++) {
		let daysInMonth = MONTH_DAYS[i];

		if (i === 1 && isLeapYear) {
			daysInMonth = 29n;
		}

		if (totalDays >= daysInMonth) {
			totalDays -= daysInMonth;
			month++;
		} else {
			break;
		}
	}

	let day = Number(totalDays + 1n);

	// --- formatting ---
	let hourStr =
		(hour < 10 ? "0" + hour : "" + hour) + ":" +
		(minute < 10 ? "0" + minute : "" + minute);

	let suffix = "th";
	if (day % 10 === 1 && day % 100 !== 11) suffix = "st";
	else if (day % 10 === 2 && day % 100 !== 12) suffix = "nd";
	else if (day % 10 === 3 && day % 100 !== 13) suffix = "rd";

	return {
		day: weekdays[weekDay] + ", " + day + suffix + " " + months[month],
		year: "Year " + year.toString().padStart(3, "0"),
		hour: hourStr
	};
}