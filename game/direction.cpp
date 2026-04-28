#include "direction.hpp"

#include <stdint.h>
#include <stdbool.h>

/**
 * Returns the value of a specific side or 4 if the side is marked as "special".
 * @param data      The 16-bit word containing the side information.
 * @param direction 0: right, 1: up, 2: left, 3: down.
 * @return          The side value (0-3) or 4 if the side is special.
 */
int direction::getSide(uint16_t data, int direction) {
	// Extract the special mode toggle and the starting side index
	bool is_special_enabled = (data >> 12) & 0x01;
	int selector = (data >> 13) & 0x03;

	// Standard mode: simply extract the 2 bits dedicated to the direction
	if (!is_special_enabled) {
		// Offset: bit 4 + (2 * direction)
		return (data >> (4 + 2 * direction)) & 0x03;
	}

	/* * SPECIAL mode enabled:
	 * We follow a chain of "special" sides starting from the selector.
	 */
	int current_check = selector;
	
	// Iterate through the linked special sides
	// Max 4 iterations to prevent infinite loops with invalid data
	for (int i = 0; i < 4; i++) {
		// If the requested direction matches a side in the special chain
		if (direction == current_check) {
			return 4;
		}

		// Read the offset (idx) stored at the current side's position
		int idx = (data >> (4 + 2 * current_check)) & 0x03;

		if (idx == 0) {
			// idx == 0 means the chain ends here; no more sides are special
			break;
		} else {
			// Calculate the next special side: (current + idx) % 4
			current_check = (current_check + idx) % 4;
		}
	}

	// If the direction wasn't found in the special chain, return its raw data value
	return (data >> (4 + 2 * direction)) & 0x03;
}





/**
 * Updates the value of a side, maintaining the linked list structure for "special" (4) values.
 */
uint16_t direction::setSide(uint16_t data, int direction, int value) {
    bool is_special_enabled = (data >> 12) & 0x01;
    int selector = (data >> 13) & 0x03;

    // 1. Identify which sides are currently "special"
    bool special_map[4] = {false, false, false, false};
    if (is_special_enabled) {
        int current = selector;
        for (int i = 0; i < 4; i++) {
            special_map[current] = true;
            int idx = (data >> (4 + 2 * current)) & 0x03;
            if (idx == 0) break;
            current = (current + idx) % 4;
        }
    }

    // 2. Check if anything actually changes
    bool was_special = special_map[direction];
    bool now_special = (value == 4);

    if (was_special == now_special && !now_special) {
        // Simple update: normal value to normal value
        data &= (uint16_t)~(0x03 << (4 + 2 * direction)); // Clear bits
        data |= (uint16_t)((value & 0x03) << (4 + 2 * direction)); // Set new value
        return data;
    }
    
    if (was_special == now_special && now_special) {
        // Both were special, nothing to change
        return data;
    }

    // 3. Update the special map
    special_map[direction] = now_special;

    // 4. Rebuild the whole structure
    // Clear all side data bits (4-11) and the special/selector flags (12-14)
    uint16_t type_and_taken = data & 0x800F; 
    uint16_t new_data = type_and_taken;

    // Find all special sides to count them and find the first one
    int first_special = -1;
    int special_count = 0;
    for (int i = 0; i < 4; i++) {
        if (special_map[i]) {
            if (first_special == -1) first_special = i;
            special_count++;
        }
    }

    if (special_count > 0) {
        // Enable special flag and set selector to the first special side
        new_data |= (1 << 12);
        new_data |= (uint16_t)(first_special << 13);

        int current = first_special;
        int processed = 0;

        while (processed < special_count) {
            processed++;
            // Find the next special side to calculate the offset (idx)
            int next_special = -1;
            for (int j = 1; j < 4; j++) {
                int candidate = (current + j) % 4;
                if (special_map[candidate]) {
                    next_special = candidate;
                    // idx is the distance to the next special side
                    new_data |= (uint16_t)(j << (4 + 2 * current));
                    break;
                }
            }

            if (next_special == -1 || processed == special_count) {
                // Last special side in the chain: idx = 0
                new_data &= (uint16_t)~(0x03 << (4 + 2 * current)); 
                break;
            }
            current = next_special;
        }
    }

    // 5. Fill in the raw values for non-special sides
    for (int i = 0; i < 4; i++) {
        if (!special_map[i]) {
            int val_to_write = (i == direction) ? value : ((data >> (4 + 2 * i)) & 0x03);
            new_data |= (uint16_t)((val_to_write & 0x03) << (4 + 2 * i));
        }
    }

    return new_data;
}