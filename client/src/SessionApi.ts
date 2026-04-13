import createModule from "../api/api.js";

import { Vector } from "./tools/Vector";
import { Rectangle } from "./tools/Rectangle";


import { ApiTakeCode } from "../../commons/ApiTakeCode"

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

class SessionApi {
	private module: any = null;
	private apiPtr: number = 0;
	private sessionId: number | null = null;

	private chunks = new Map<number, Chunk>();

	async init() {
		this.module = await createModule();
		this.apiPtr = this.module._Api_createApi(1);
	}

	cleanup() {
		if (!this.module || !this.apiPtr) return;

		if (this.sessionId !== null) {
			this.module._Api_deleteSession(this.apiPtr, this.sessionId);
			this.sessionId = null;
		}

		this.module._Api_deleteApi(this.apiPtr);

		this.apiPtr = 0;
		this.module = null;
	}

	createSession() {
		if (this.sessionId !== null) {
			throw new Error("Session already exists");
		}

		this.sessionId = this.module._Api_createSession(this.apiPtr);
	}

	deleteSession() {
		if (this.sessionId === null) {
			throw new Error("No session to delete");
		}

		this.module._Api_deleteSession(this.apiPtr, this.sessionId);
		this.sessionId = null;
	}

	private run(code: number, args: any = 0) {
		return this.module._Api_take(this.apiPtr, this.sessionId, code, args);
	}


	private chunkKey(cx: number, cy: number): number {
		return (cx << 16) ^ (cy & 0xffff);
	}

	private getChunk(cx: number, cy: number): Chunk {
		const key = this.chunkKey(cx, cy);

		let chunk = this.chunks.get(key);
		if (!chunk) {
			chunk = new Chunk();
			this.chunks.set(key, chunk);
		}

		return chunk;
	}

	private worldToChunk(x: number, y: number) {
		return {
			cx: Math.floor(x / Chunk.SIZE),
			cy: Math.floor(y / Chunk.SIZE),
			lx: ((x % Chunk.SIZE) + Chunk.SIZE) % Chunk.SIZE,
			ly: ((y % Chunk.SIZE) + Chunk.SIZE) % Chunk.SIZE,
		};
	}



	takeCoords(): Rectangle {
		const ptr = this.run(ApiTakeCode.COPY_COORDS) >> 2;

		const x = this.module.HEAP32[ptr + 0];
		const y = this.module.HEAP32[ptr + 1];
		const w = this.module.HEAP32[ptr + 2];
		const h = this.module.HEAP32[ptr + 3];

		this.run(ApiTakeCode.FREE_COORDS);

		return { x, y, w, h };
	}

	createCellGrid() {
		this.chunks.clear();

		const arg = this.module._malloc(4 * 4);
		const view = this.module.HEAPU32.subarray(arg >> 2);
		const rect = this.takeCoords();

		view[0] = rect.x;
		view[1] = rect.y;
		view[2] = rect.w;
		view[3] = rect.h;

		const ptr = this.run(ApiTakeCode.MAKE_MAP, arg) >> 1;
		this.module._free(arg);

		const viewCells = this.module.HEAPU16.subarray(
			ptr,
			ptr + rect.w * rect.h
		);

		for (let y = 0; y < rect.h; y++) {
			for (let x = 0; x < rect.w; x++) {
				const wx = rect.x + x;
				const wy = rect.y + y;

				const { cx, cy, lx, ly } = this.worldToChunk(wx, wy);
				const chunk = this.getChunk(cx, cy);

				chunk.set(lx, ly, viewCells[y * rect.w + x]);
			}
		}
	}

	updateCells(viewX: number, viewY: number) {
		const baseCX = Math.floor(viewX / Chunk.SIZE);
		const baseCY = Math.floor(viewY / Chunk.SIZE);

		// 3x3 chunk area
		const bx = (baseCX - 1) * Chunk.SIZE;
		const by = (baseCY - 1) * Chunk.SIZE;
		const bw = 3 * Chunk.SIZE;
		const bh = 3 * Chunk.SIZE;

		const argPtr = this.module._malloc(4 * 4);
		const arg = this.module.HEAPU32.subarray(argPtr >> 2);

		arg[0] = bx;
		arg[1] = by;
		arg[2] = bw;
		arg[3] = bh;

		const ptr = this.run(ApiTakeCode.TAKE_MAP_EDITS, argPtr) >> 2;
		this.module._free(argPtr);


		const len = this.module.HEAPU32[ptr];
		let cursor = ptr + 1;

		for (let i = 0; i < len; i++) {
			const packed = this.module.HEAPU32[cursor++];

			const dx = (packed >> 24) & 0xff;
			const dy = (packed >> 16) & 0xff;
			const data = packed & 0xffff;

			const wx = bx + dx;
			const wy = by + dy;

			const { cx, cy, lx, ly } = this.worldToChunk(wx, wy);
			const chunk = this.getChunk(cx, cy);

			chunk.set(lx, ly, data);
		}

		this.run(ApiTakeCode.RLSE_MAP_EDITS);
	}

	getCell(x: number, y: number) {
		const { cx, cy, lx, ly } = this.worldToChunk(x, y);
		const chunk = this.getChunk(cx, cy);
		return chunk.get(lx, ly);
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


	placeRoad(x: number, y: number) {
		const arg = this.module._malloc(4 * 2);
		const view = this.module.HEAPU32.subarray(arg >> 2);

		view[0] = x;
		view[1] = y;

		this.run(ApiTakeCode.PLACE_ROAD, arg);

		this.module._free(arg);
	}

	takeCars() {
		const srcPtr = this.run(ApiTakeCode.COPY_CARS) >> 2;

		const number = this.module.HEAPU32[srcPtr];

		this.run(ApiTakeCode.FREE_CARS);
	}
}

export const api = new SessionApi();
api.init();

