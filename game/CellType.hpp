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
     * +04: (empty)
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
    
    COUNT
};