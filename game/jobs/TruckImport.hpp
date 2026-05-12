#pragma once

#include "TruckImportId.hpp"

struct TruckImport {
	int x;
	int y;
	TruckImportId id; // negative means destination
};
