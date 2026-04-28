export namespace direction {
	export function getSide(data: number, direction: number): number {
		const dir1 = (data >> 10) & 0x3;
		const dir2 = (data >> 12) & 0x3;
		const side1 = (data >> 4) & 0x7;
		const side2 = (data >> 7) & 0x7;

		if (side1 !== 0 && dir1 === direction) return side1;
		if (side2 !== 0 && dir2 === direction) return side2;

		return 0;
	}

	export function setSide(data: number, direction: number, value: number): number {
		const dir1 = (data >> 10) & 0x3;
		const dir2 = (data >> 12) & 0x3;
		const side1 = (data >> 4) & 0x7;
		const side2 = (data >> 7) & 0x7;

		if (side1 !== 0 && dir1 === direction) {
			data &= ~(0x7 << 4);
			data |= (value & 0x7) << 4;
			return data;
		}
		if (side2 !== 0 && dir2 === direction) {
			data &= ~(0x7 << 7);
			data |= (value & 0x7) << 7;
			return data;
		}

		if (side1 === 0) {
			data &= ~(0x7 << 4);
			data &= ~(0x3 << 10);
			data |= (value & 0x7) << 4;
			data |= (direction & 0x3) << 10;
			return data;
		}
		if (side2 === 0) {
			data &= ~(0x7 << 7);
			data &= ~(0x3 << 12);
			data |= (value & 0x7) << 7;
			data |= (direction & 0x3) << 12;
			return data;
		}

		return 0;
	}
}