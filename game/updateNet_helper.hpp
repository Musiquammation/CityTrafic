#pragma once

#include "declarations.hpp"
#include <stdint.h>

struct Client;

uint32_t* updateNet_helper_write(
	Game& game,
	int x, int y, int w, int h,
	uint8_t clientRequestId,
	int money,
	bool updateClientJobs, int cellsLayerId,
	int mapPrecision,
	int rx0, int ry0, int rx1, int ry1
);

void  updateNet_helper_read(Game& game, void* args);
