#pragma once

#include "declarations.hpp"

bool makeCarPath(const Map& map, PathHandler<true>& path, int startY, int destX, int destY);
char* makePedestranPath(const Map& map, int startX, int startY, int destX, int destY);
