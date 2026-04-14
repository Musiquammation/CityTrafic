import os from 'os'
import fs from 'fs'

import { ApiTakeCode } from "../commons/ApiTakeCode"

// @ts-ignore
import createModule from "./api/api";

const wasmBinary = fs.readFileSync("server/api/api.wasm");

const module = await createModule({
	wasmBinary,
});

interface Grid {
	ptr: number;
	view: Uint16Array;
}


class Session {
	readonly id: number;
	grid: Grid | null = null;
	
	constructor(id: number) {
		this.id = id;
	}
}

class MatchApi {
	private apiPtr: number = 0;

	constructor() {
		const numThreads = Math.max(1, os.cpus().length - 1);
		this.apiPtr = module._Api_createApi(numThreads);
	}

	createSession(): Session {
		const id = module._Api_createSession(this.apiPtr);
		return new Session(id);
	}

	deleteSession(s: Session) {
		if (s.grid) {
			module._free(s.grid.ptr);
		}

		module._Api_deleteSession(this.apiPtr, s.id);
	}


		
	private run(sessionId: number, code: number, args: any = 0) {
		return module._Api_take(this.apiPtr, sessionId, code, args);
	}

	takeCoords(s: Session) {
		const ptr = this.run(s.id, ApiTakeCode.COPY_COORDS) >> 2;

		const x = module.HEAP32[ptr + 0];
		const y = module.HEAP32[ptr + 1];
		const w = module.HEAP32[ptr + 2];
		const h = module.HEAP32[ptr + 3];

		this.run(s.id, ApiTakeCode.FREE_COORDS);

		return { x, y, w, h };
	}


	createCellGrid(s: Session) {
		if (s.grid) {
			// free previous grid
			module._free(s.grid.ptr);
		}

		const rect = this.takeCoords(s);
		const argPtr = module._malloc(4 * 4);
		const argView = module.HEAPU32.subarray(argPtr >> 2);
		argView[0] = rect.x;
		argView[1] = rect.y;
		argView[2] = rect.w;
		argView[3] = rect.h;


		const ptr = this.run(s.id, ApiTakeCode.MAKE_MAP, argPtr);
		const view = new Uint16Array(
			module.HEAPU16.buffer,
			ptr,
			rect.w * rect.h
		);
		
		s.grid = {ptr, view};
		return view;
	}

	updateCells(viewX: number, viewY: number, rangeW: number, rangeH: number) {
		
	}

	collectArea(x: number, y: number, w: number, h: number) {

	}
}


export const api = new MatchApi();

