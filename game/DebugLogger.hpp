#pragma once


class DebugLogger {
public:
	DebugLogger(const char* prefix, bool enable = true);

	void operator()(const char* fmt, ...) const;

private:
	const char* prefix;
	bool enable;
};

extern DebugLogger printWarn;