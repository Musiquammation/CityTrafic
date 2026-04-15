
import { ApiTakeCode } from "../commons/ApiTakeCode"
import { Match } from "./Match"


// const FRAME_DELAY = 16;
const FRAME_DELAY = 1000;

export class MatchApi {
	private apiPtr: number = 0;
	private frameInterval;
	module: any;

	constructor(module: any, indexStart: number, indexSpacing: number) {
		this.module = module;
		this.apiPtr = this.module._Api_createApi(indexStart, indexSpacing);

		this.frameInterval = setInterval(() => {
			this.runFrames();
		}, FRAME_DELAY);
	}


	delete() {
		clearInterval(this.frameInterval);
		this.module._Api_deleteApi(this.apiPtr);
	}

	createMatch(): Match {
		const id = this.module._Api_createSession(this.apiPtr);
		return new Match(id, this.createCellGrid(id));
	}

	deleteMatch(s: Match) {
		if (s.grid) {
			this.module._free(s.grid.ptr);
		}

		this.module._Api_deleteSession(this.apiPtr, s.id);
	}

	private runFrames() {
		this.module._Api_runFrames(this.apiPtr);
	}



		
	private run(sessionId: number, code: number, args: any = 0) {
		return this.module._Api_take(this.apiPtr, sessionId, code, args);
	}

	takeCoords(id: number) {
		const ptr = this.run(id, ApiTakeCode.COPY_COORDS) >> 2;

		const x = this.module.HEAP32[ptr + 0];
		const y = this.module.HEAP32[ptr + 1];
		const w = this.module.HEAP32[ptr + 2];
		const h = this.module.HEAP32[ptr + 3];

		this.run(id, ApiTakeCode.FREE_COORDS);

		return { x, y, w, h };
	}

	private createCellGrid(id: number) {
		const rect = this.takeCoords(id);
		const argPtr = this.module._malloc(4 * 4);
		const argView = this.module.HEAPU32.subarray(argPtr >> 2);
		argView[0] = rect.x;
		argView[1] = rect.y;
		argView[2] = rect.w;
		argView[3] = rect.h;


		const ptr = this.run(id, ApiTakeCode.MAKE_MAP, argPtr);
		const view = new Uint16Array(
			this.module.HEAPU16.buffer,
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
		// free previous grid
		this.module._free(s.grid.ptr);

		s.grid = this.createCellGrid(s.id);
	}

	updateCells(s: Match, x0: number, y0: number, width: number, height: number) {
		if (!s.grid)
			throw new Error("Missing grid");

		const argPtr = this.module._malloc(4 * 4);
		const arg = this.module.HEAPU32.subarray(argPtr >> 2);

		arg[0] = x0;
		arg[1] = y0;
		arg[2] = width;
		arg[3] = height;

		const ptr = this.run(s.id, ApiTakeCode.TAKE_MAP_EDITS, argPtr) >> 2;
		this.module._free(argPtr);


		const len = this.module.HEAPU32[ptr];
		let cursor = ptr + 1;
		const sx = s.grid.mapX;
		const sy = s.grid.mapY;
		const sw = s.grid.mapW;
		const gridView = s.grid.view;

		for (let i = 0; i < len; i++) {
			const packed = this.module.HEAPU32[cursor++];

			const dx = (packed >> 24) & 0xff;
			const dy = (packed >> 16) & 0xff;
			const data = packed & 0xffff;

			const x = x0 + dx;
			const y = y0 + dy;

			const idx = (y - sy) * sw + (x - sx);

			gridView[idx] = data;
		}
	}

	collectArea(s: Match, x: number, y: number, w: number, h: number) {
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

		return {
			transfered: [result.buffer],
			result
		};
	}

	placeSingleRoad(s: Match, x: number, y: number) {
		const argPtr = this.module._malloc(4 * 2);
		const argView = this.module.HEAP32.subarray(argPtr >> 2);

		argView[0] = x;
		argView[1] = y;
		this.run(s.id, ApiTakeCode.PLACE_SINGLE_ROAD, argPtr);
		
		this.module._free(argPtr);
	}
}



