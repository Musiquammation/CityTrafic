#pragma once

#include <stdio.h>

template<const char* Prefix, bool enable>
auto generateLog() {
	return []<typename... Args>(const char* fmt, Args... args) {
		if constexpr (!enable) {
			return;
		}

		printf("[%s] ", Prefix);
		printf(fmt, args...);
		printf("\n");
	};
}