#pragma once

#include "declarations.hpp"
#include <stdint.h>

uint32_t* updateNet_helper_write(Game& game, int x, int y, int w, int h, uint8_t clientRequestId, int money);
void  updateNet_helper_read(Game& game, void* args);
