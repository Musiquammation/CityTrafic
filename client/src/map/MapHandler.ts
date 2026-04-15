import { Chunk } from "./Chunk";

export class MapHandler {
	private readonly chunks = new Map<number, Chunk>();


	private chunkKey(cx: number, cy: number): number {
		return (cx << 16) ^ (cy & 0xffff);
	}
	
	getChunk(cx: number, cy: number): Chunk {
		const key = this.chunkKey(cx, cy);

		let chunk = this.chunks.get(key);
		if (!chunk) {
			chunk = new Chunk();
			this.chunks.set(key, chunk);
		}

		return chunk;
	}

	getChunkAt(wx: number, wy: number) {
		const { cx, cy, lx, ly } = this.worldToChunk(wx, wy);
		const chunk = this.getChunk(cx, cy);
		return {chunk, lx, ly};
	}

	worldToChunk(x: number, y: number) {
		return {
			cx: Math.floor(x / Chunk.SIZE),
			cy: Math.floor(y / Chunk.SIZE),
			lx: ((x % Chunk.SIZE) + Chunk.SIZE) % Chunk.SIZE,
			ly: ((y % Chunk.SIZE) + Chunk.SIZE) % Chunk.SIZE,
		};
	}

	getCell(x: number, y: number) {
		const { chunk, lx, ly } = this.getChunkAt(x, y);
		return chunk.get(lx, ly);
	}

	setCell(x: number, y: number, data: number) {
		const { chunk, lx, ly } = this.getChunkAt(x, y);
		chunk.set(lx, ly, data);
	}

	*getChunks(viewX: number, viewY: number, rangeW: number, rangeH: number) {
		const halfW = Math.floor(rangeW / 2);
		const halfH = Math.floor(rangeH / 2);

		const minX = viewX - halfW;
		const minY = viewY - halfH;
		const maxX = viewX + halfW;
		const maxY = viewY + halfH;

		const minCX = Math.floor(minX / Chunk.SIZE) - 1;
		const minCY = Math.floor(minY / Chunk.SIZE) - 1;
		const maxCX = Math.floor(maxX / Chunk.SIZE) + 1;
		const maxCY = Math.floor(maxY / Chunk.SIZE) + 1;


		for (let cy = minCY; cy <= maxCY; cy++) {
			for (let cx = minCX; cx <= maxCX; cx++) {
				yield ({
					x: cx * Chunk.SIZE,
					y: cy * Chunk.SIZE,
					cells: this.getChunk(cx, cy).cells,
				});
			}
		}
	}

	*getLineBuffer(x: number, y: number, w: number) {
		if (w <= 0) return;

		let remaining = w;
		let wx = x;

		while (remaining > 0) {
			const { cx, cy, lx, ly } = this.worldToChunk(wx, y);
			const chunk = this.getChunk(cx, cy);

			const available = Chunk.SIZE - lx;

			const take = Math.min(remaining, available);

			const start = ly * Chunk.SIZE + lx;
			const end = start + take;

			// Share array
			yield chunk.cells.subarray(start, end);

			// Move
			wx += take;
			remaining -= take;
		}
	}

	clear() {
		this.chunks.clear();
	}
}