#pragma once

#include <stdint.h>

namespace direction {
    int getSide(uint16_t data, int direction);
    uint16_t setSide(uint16_t data, int direction, int value);
}

