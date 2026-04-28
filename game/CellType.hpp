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
     * +04: data
     * +12: special?
     * +13: selector
     * +15: (taken)
     * 
     * If special is diseabled, data is:
     *   +04: right
     *   +06: up
     *   +08: left
     *   +10: down
     *   +12
     * 
     * else, 
     *   special-th side is special.
     *   by reading idx = data[(4+2*side):2],
     *   
     *   while True:
     *     if idx==0:
     *       only this side is special
     *       break
     *     else:
     *       this side and (side+idx)%4 is special
     * 
     */
    DIRECTION,
    
    COUNT
};