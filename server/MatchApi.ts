import os from 'os'
import fs from 'fs'

import { ApiTakeCode } from "../commons/ApiTakeCode"

// @ts-ignore
import createModule from "./api/api";
import { Match } from './Match';

const wasmBinary = fs.readFileSync("server/api/api.wasm");

const module = await createModule({
	wasmBinary,
});



class MatchApi {
	private apiPtr: number = 0;

	constructor() {
		const numThreads = Math.max(1, os.cpus().length - 1);
		this.apiPtr = module._Api_createApi(numThreads);
	}

	createMatch(): Match {
		const id = module._Api_createSession(this.apiPtr);
		return new Match(id, this.createCellGrid(id));
	}

	deleteMatch(s: Match) {
		if (s.grid) {
			module._free(s.grid.ptr);
		}

		module._Api_deleteSession(this.apiPtr, s.id);
	}


		
	private run(sessionId: number, code: number, args: any = 0) {
		return module._Api_take(this.apiPtr, sessionId, code, args);
	}

	takeCoords(id: number) {
		const ptr = this.run(id, ApiTakeCode.COPY_COORDS) >> 2;

		const x = module.HEAP32[ptr + 0];
		const y = module.HEAP32[ptr + 1];
		const w = module.HEAP32[ptr + 2];
		const h = module.HEAP32[ptr + 3];

		this.run(id, ApiTakeCode.FREE_COORDS);

		return { x, y, w, h };
	}

	private createCellGrid(id: number) {
		const rect = this.takeCoords(id);
		const argPtr = module._malloc(4 * 4);
		const argView = module.HEAPU32.subarray(argPtr >> 2);
		argView[0] = rect.x;
		argView[1] = rect.y;
		argView[2] = rect.w;
		argView[3] = rect.h;


		const ptr = this.run(id, ApiTakeCode.MAKE_MAP, argPtr);
		const view = new Uint16Array(
			module.HEAPU16.buffer,
			ptr,
			rect.w * rect.h
		);
		
		return {
			ptr,
			view,
			mapX: rect.x,
			mapY: rect.y,
			mapW: rect.w,
			mapH: rect.h
		};
	}

	appendCellGrid(s: Match) {
		if (s.grid) {
			// free previous grid
			module._free(s.grid.ptr);
		}

		const grid = this.createCellGrid(s.id);
		s.grid = grid;
	}

	updateCells(s: Match, x0: number, y0: number, width: number, height: number) {
		if (!s.grid)
			throw new Error("Missing grid");

		const argPtr = module._malloc(4 * 4);
		const arg = module.HEAPU32.subarray(argPtr >> 2);

		arg[0] = x0;
		arg[1] = y0;
		arg[2] = width;
		arg[3] = height;

		const ptr = this.run(s.id, ApiTakeCode.TAKE_MAP_EDITS, argPtr) >> 2;
		module._free(argPtr);


		const len = module.HEAPU32[ptr];
		let cursor = ptr + 1;
		const sx = s.grid.mapX;
		const sy = s.grid.mapY;
		const sw = s.grid.mapW;
		const gridView = s.grid.view;

		for (let i = 0; i < len; i++) {
			const packed = module.HEAPU32[cursor++];

			const dx = (packed >> 24) & 0xff;
			const dy = (packed >> 16) & 0xff;
			const data = packed & 0xffff;

			const x = x0 + dx;
			const y = y0 + dy;

			const idx = (y - sy) * sw + (x - sx);

			gridView[idx] = data;
		}
	}

	collectArea(s: Match, x: number, y: number, w: number, h: number): Uint16Array {
		if (!s.grid)
			throw new Error("Missing grid");

		const { view, mapX, mapY, mapW, mapH } = s.grid;

		if (
			x < mapX || y < mapY ||
			x + w > mapX + mapW ||
			y + h > mapY + mapH
		) {
			throw new Error("Out of bounds");
		}

		const result = new Uint16Array(w * h);

		const startX = x - mapX;
		const startY = y - mapY;

		for (let row = 0; row < h; row++) {
			const srcOffset = (startY + row) * mapW + startX;
			const dstOffset = row * w;

			result.set(
				view.subarray(srcOffset, srcOffset + w),
				dstOffset
			);
		}

		return result;
	}
}


export const api = new MatchApi();

