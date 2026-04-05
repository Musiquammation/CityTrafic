#pragma once

#include <stdint.h>

/**
 * General format:
 * 
 * +00: type
 * +04: data
 * +15: hasCar?
 * +16
 */
typedef uint16_t cell_t;

class Cell {
public:
    cell_t data;


    void setCarOn() {
        this->data |= 1<<15;
    }

    void setCarOff() {
        this->data &= ~(1<<15);
    }

    uint16_t hasCar() {
        return this->data & (1<<15);
    }
};