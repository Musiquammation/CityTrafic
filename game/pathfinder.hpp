#pragma once

#include "declarations.hpp"
#include "Direction.hpp"

bool makeCarPath(const Map& map, PathHandler<true>& path, int startX, int startY, int destX, int destY, Direction startDir);
char* makePedestranPath(const Map& map, int startX, int startY, int destX, int destY);
