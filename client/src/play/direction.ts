export namespace direction {

	export function getSide(data: number, direction: number): number {
		const dir1 = (data >> 10) & 0x3;
		const dir2 = (data >> 12) & 0x3;

		if (direction === dir1) {
			return (data >> 4) & 0x7;
		}  else if (direction === dir2) {
			return (data >> 7) & 0x7;
		}

		return -1;
	}

	export function setSide(data: number, direction: number, value: number): number {
		// Extract existing directions
		const dir1 = (data >> 10) & 0x3;
		const dir2 = (data >> 12) & 0x3;

		// Extract existing side values
		const side1 = (data >> 4) & 0x7;
		const side2 = (data >> 7) & 0x7;

		// --- Update existing slot ---
		if (side1 !== 0 && dir1 === direction) {
			data &= ~(0x7 << 4);
			data |= (value & 0x7) << 4;
			return data;
		} 
		else if (side2 !== 0 && dir2 === direction) {
			data &= ~(0x7 << 7);
			data |= (value & 0x7) << 7;
			return data;
		}

		// --- Fill empty slot if direction doesn't exist ---
		if (side1 === 0) {
			data |= (value & 0x7) << 4;
			data |= (direction & 0x3) << 10;
			return data;
		}  else if (side2 === 0) {
			data |= (value & 0x7) << 7;
			data |= (direction & 0x3) << 12;
			return data;
		}

		return 0; // Failure
	}
}