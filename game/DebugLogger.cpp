#include "DebugLogger.hpp"

#include <cstdarg>
#include <cstdio>
#include <vector>

DebugLogger::DebugLogger(const char* prefix, bool enable)
	: prefix(prefix), enable(enable) {}

void DebugLogger::operator()(const char* fmt, ...) const {
	if (!enable)
		return;

	va_list args;
	va_start(args, fmt);

	// size estimation
	va_list args_copy;
	va_copy(args_copy, args);
	int size = std::vsnprintf(nullptr, 0, fmt, args_copy);
	va_end(args_copy);

	if (size <= 0) {
		va_end(args);
		return;
	}

	std::vector<char> buffer(size + 1);
	std::vsnprintf(buffer.data(), buffer.size(), fmt, args);

	va_end(args);

	std::printf("[%s] %s", prefix, buffer.data());
}