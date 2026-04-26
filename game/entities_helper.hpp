#pragma once

#include "declarations.hpp"
#include <stdint.h>

uint32_t* entities_helper_make(Game& game, int x, int y, int w, int h);
void  entities_helper_read(Game& game, void* args);
