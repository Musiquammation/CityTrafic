#pragma once

enum class CellType {
    /**
     * +00: (type)
     * +04: code
     * +15: (taken)
     */
    NONE,

    /**
     * +00: (type)
     * +04: right?
     * +05: up?
     * +06: left?
     * +07: down?
     * +08: (empty)
     * +15: (taken)
     */
    ROAD,

    /**
     * +00: (type)
     * +04: building type
     * +15: (taken)
     */
    BUILDING,


    /**
     * +00: (type)
     * +04: dx
     * +08: dy
     * +12: jumpX?
     * +13: jumpY?
     * +14: entry?
     * +15: (taken)
     */
    LINK,

    /**
     * +00: (type)
     * +04: reserved?
     * +05: (empty)
     * +15: (taken)
     */
    PARKING,


    /**
     * +00: (type)
     * +04: first:  side
     * +07: second: side
     * +10: first:  direction
     * +12: second: direction
     * +14: (empty)
     * +15: (taken)
     * 
     * 
     * for side:
     *   0: nothing
     *   1: front
     *   2: right
     *   3: left
     *   4: front-right
     *   5: front-left
     *   6: left-right
     *   7: all
     */
    DIRECTION,
    
    COUNT
};