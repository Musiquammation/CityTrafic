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


		// Create layer (at 0)
		this.run(ApiTakeCode.PUSH_LAYER);


		this.createCellGrid();
	}

	cleanup() {
		if (!this.module || !this.apiPtr) return;

		this.module._Api_deleteSession(this.apiPtr, 0);

		this.module._Api_deleteApi(this.apiPtr);

		this.apiPtr = 0;
		this.module = null;
	}

	freeBuffer() {
		this.run(-1, ApiTakeCode.FREE_BUFFER);
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

		const argPtr = this.module._malloc(4 * 5);
		const arg = this.module.HEAPU32.subarray(argPtr >> 2);

		arg[0] = bx;
		arg[1] = by;
		arg[2] = bw;
		arg[3] = bh;
		arg[4] = 0; // layer=0

		const ptr = this.run(ApiTakeCode.MAKE_MAP_EDITS, argPtr) >> 2;
		this.module._free(argPtr);

		let cursor = ptr+1;

		for (let rangeCount = this.module.HEAPU32[cursor++]; rangeCount; rangeCount--) {
			const x0 = this.module.HEAP32[cursor++];
			const y0 = this.module.HEAP32[cursor++];


			for (let count = this.module.HEAPU32[cursor++]; count; count--) {
				const packed = this.module.HEAPU32[cursor++];

	
				const dx = (packed >> 24) & 0xff;
				const dy = (packed >> 16) & 0xff;
				const data = packed & 0xffff;
	
				const wx = x0 + dx;
				const wy = y0 + dy;

				
				const {chunk, lx, ly} = this.map.getChunkAt(wx, wy);
				
				chunk.set(lx, ly, data);
			}
		}
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

			heap.set(buffer, dstOffset);
		}
	}


	placeRoad(x: number, y: number) {
		const arg = this.module._malloc(4 * 2);
		const view = this.module.HEAPU32.subarray(arg >> 2);

		view[0] = x;
		view[1] = y;

		this.run(ApiTakeCode.PLACE_SINGLE_ROAD, arg);

		this.module._free(arg);
	}

	takeCars() {
		const srcPtr = this.run(ApiTakeCode.COPY_CARS) >> 2;

		const number = this.module.HEAPU32[srcPtr];

		/// TODO: complete
	}

	getChunks(viewX: number, viewY: number, rangeW: number, rangeH: number) {
		const chunks = [];
		const transfered: Transferable[] = [];

		for (const chunk of this.map.getChunks(viewX, viewY, rangeW, rangeH)) {
			const cells = new Uint16Array(chunk.cells);

			chunks.push({
				x: chunk.x,
				y: chunk.y,
				cells
			});

			transfered.push(cells.buffer);
		}

		return {
			result: chunks,
			transfered
		}
	}

	applyEdits(array: Uint32Array) {
		// Copy array
		const argPtr = this.module._malloc(array.length * 4);
		this.module.HEAPU32.set(array, argPtr >> 2);
		
		// Apply edits
		this.run(ApiTakeCode.APPLY_EDITS, argPtr);

		// Free argPtr
		this.module._free(argPtr);
	}


	performGameCommand(data: ArrayBuffer) {
		const argPtr = this.module._malloc(data.byteLength);

		this.module.HEAPU32.set(new Uint32Array(data), argPtr>>2);

		this.run(ApiTakeCode.GAME_COMMAND, argPtr);
		this.module._free(argPtr);
	}

	readEntities(array: Uint8Array) {
		// Copy array
		const argPtr = this.module._malloc(array.length);
		this.module.HEAPU8.set(array, argPtr);
		
		// Apply edits
		this.run(ApiTakeCode.READ_ENTITIES, argPtr);

		// Free argPtr
		this.module._free(argPtr);
	}
}

