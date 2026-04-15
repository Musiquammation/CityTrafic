export class Chunk {
	static readonly SIZE = 32;

	cells: Uint16Array;

	constructor() {
		this.cells = new Uint16Array(Chunk.SIZE * Chunk.SIZE);
	}

	get(x: number, y: number): number {
		return this.cells[y * Chunk.SIZE + x];
	}

	set(x: number, y: number, v: number) {
		this.cells[y * Chunk.SIZE + x] = v;
	}
}