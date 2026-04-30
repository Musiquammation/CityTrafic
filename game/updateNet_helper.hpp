#pragma once

#include "declarations.hpp"
#include <stdint.h>

struct Client;

uint32_t* updateNet_helper_write(
	Game& game,
	int x, int y, int w, int h,
	uint8_t clientRequestId,
	int money,
	bool updateClientJobs
);

void  updateNet_helper_read(Game& game, void* args);
