import createModule from "../../wasm/api.js"

import { Rectangle } from "../tools/Rectangle";

import { ApiTakeCode } from "../../../commons/ApiTakeCode"
import { Chunk } from "./Chunk";
import { MapHandler } from "./MapHandler.ts"

// @ts-ignore
import { Buffer } from "buffer";



export class ClientApi {
	private module: any = null;
	private apiPtr: number = 0;
	private map = new MapHandler();

	async init(wasmPath: string) {
		this.module = await createModule({
			locateFile(path: string) {
				if (path.endsWith(".wasm")) {
					return new URL(wasmPath, import.meta.url).href;
				}
				return path;
			}


		});
		this.apiPtr = this.module._Api_createApi(0, 0);

		this.createCellGrid();
	}

	cleanup() {
		if (!this.module || !this.apiPtr) return;

		this.module._Api_deleteSession(this.apiPtr, 0);

		this.module._Api_deleteApi(this.apiPtr);

		this.apiPtr = 0;
		this.module = null;
	}

	createSession() {
		this.module._Api_createSession(this.apiPtr);
	}

	deleteSession() {
		this.module._Api_deleteSession(this.apiPtr, 0);
	}

	private run(code: number, args: any = 0) {
		return this.module._Api_take(this.apiPtr, 0, code, args);
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
		this.map.clear();

		const rect = this.takeCoords();
		
		const arg = this.module._malloc(4 * 4);
		const view = this.module.HEAPU32.subarray(arg >> 2);
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

				const {chunk, lx, ly} = this.map.getChunkAt(wx, wy);

				chunk.set(lx, ly, viewCells[y * rect.w + x]);
			}
		}

		// Map copy is not used
		this.module._free(ptr);
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

			const {chunk, lx, ly} = this.map.getChunkAt(wx, wy);

			chunk.set(lx, ly, data);
		}

		this.run(ApiTakeCode.RLSE_MAP_EDITS);
	}

	

	setArea(x0: number, y0: number, w: number, h: number, buffer: Buffer) {
		const reader = new Uint16Array(buffer);

		let offset = 0;
		// Fill buffers
		let buffers = new Array<Uint16Array>(h);
		for (let y = y0, yf = y0+h; y < yf; y++) {
			const buffer = new Uint16Array(w);
			buffers[y-y0] = buffer;

			let i = 0;

			for (const line of this.map.getLineBuffer(x0, y, w)) {
				for (let j = 0; j < line.length; j++) {
					const data = reader[offset++];
					line[j] = data;
					buffer[i++] = data;
				}
			}
		}

		// Take coordonates
		const rect = this.takeCoords();
		const rx = rect.x;
		const ry = rect.x;
		const rw = rect.w;



		// Copy buffers in C++ map
		const mapPtr = this.run(ApiTakeCode.TAKE_MAP_PTR) >> 1;
		const heap = this.module.HEAPU16;

		for (let y = y0, yf = y0 + h; y < yf; y++) {
			const buffer = buffers[y - y0];

			const dstOffset =
				mapPtr +
				(y - ry) * rw +
				(x0 - rx);

			console.log(x0, y);
			heap.set(buffer, dstOffset);
		}


		this.run(ApiTakeCode.RLSE_MAP_PTR);
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

	getChunks(viewX: number, viewY: number, rangeW: number, rangeH: number) {
		return Array.from(this.map.getChunks(
			viewX, viewY, rangeW, rangeH));
		
	}
}

