#ifndef SUB_DIRECTION_HPP
#define SUB_DIRECTION_HPP

#include <stdint.h>

namespace direction {
    int getSide(uint16_t data, int direction);
    uint16_t setSide(uint16_t data, int direction, int value);
}


#endif