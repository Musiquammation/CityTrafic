
import { ApiTakeCode } from "../commons/ApiTakeCode"
import { Client } from "./Client";



interface ApiMatch {
	ptr: number;
	
	mapX: number;
	mapY: number;
	mapW: number;
	mapH: number;
}





// const FRAME_DELAY = 16;
const FRAME_DELAY = 1000;

export class MatchApi {
	private matchs = new Map<number, ApiMatch>();
	private apiPtr: number = 0;
	private frameInterval;
	private module: any;

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

	createMatch() {
		const id = this.module._Api_createSession(this.apiPtr);
		const match = this.createApiMatch(id);
		this.matchs.set(id, match);
		return {
			id,
			mapX: match.mapX,
			mapY: match.mapY,
			mapW: match.mapW,
			mapH: match.mapH
		};
	}

	deleteMatch(id: number) {
		const m = this.matchs.get(id)!;
		this.module._free(m.ptr);

		this.module._Api_deleteSession(this.apiPtr, id);
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

	private createApiMatch(id: number): ApiMatch {
		const rect = this.takeCoords(id);
		const argPtr = this.module._malloc(4 * 4);
		const argView = this.module.HEAPU32.subarray(argPtr >> 2);
		argView[0] = rect.x;
		argView[1] = rect.y;
		argView[2] = rect.w;
		argView[3] = rect.h;


		const ptr = this.run(id, ApiTakeCode.MAKE_MAP, argPtr);
		
		return {
			ptr,
			mapX: rect.x,
			mapY: rect.y,
			mapW: rect.w,
			mapH: rect.h
		};
	}

	appendCellGrid(id: number) {
		const m = this.matchs.get(id);
		if (m) {
			this.module._free(m.ptr);
		}

		const m2 = this.createApiMatch(id);
		this.matchs.set(id, m2);
		return m2;
	}

	updateCells(id: number, x0: number, y0: number, width: number, height: number) {
		const m = this.matchs.get(id)!;

		const argPtr = this.module._malloc(4 * 4);
		const arg = this.module.HEAPU32.subarray(argPtr >> 2);

		arg[0] = x0;
		arg[1] = y0;
		arg[2] = width;
		arg[3] = height;

		const ptr = this.run(id, ApiTakeCode.TAKE_MAP_EDITS, argPtr) >> 2;
		this.module._free(argPtr);


		const len = this.module.HEAPU32[ptr];
		let cursor = ptr + 1;
		const sx = m.mapX;
		const sy = m.mapY;
		const sw = m.mapW;
		const gridPtr = m.ptr >> 1;

		for (let i = 0; i < len; i++) {
			const packed = this.module.HEAPU32[cursor++];

			const dx = (packed >> 24) & 0xff;
			const dy = (packed >> 16) & 0xff;
			const data = packed & 0xffff;

			const x = x0 + dx;
			const y = y0 + dy;

			const idx = (y - sy) * sw + (x - sx);

			this.module.HEAPU16[gridPtr+idx] = data;
		}
	}

	collectArea(id: number, x: number, y: number, w: number, h: number) {
		const m = this.matchs.get(id)!;

		const { mapX, mapY, mapW, mapH } = m;

		if (
			x < mapX || y < mapY ||
			x + w > mapX + mapW ||
			y + h > mapY + mapH
		) {
			throw new Error("Out of bounds");
		}

		const gridPtr = m.ptr >> 1;
		const view = this.module.HEAPU16.subarray(
			gridPtr,
			gridPtr + mapW * mapH
		);

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

	placeSingleRoad(id: number, x: number, y: number) {
		const m = this.matchs.get(id)!;
		const argPtr = this.module._malloc(4 * 2);
		const argView = this.module.HEAP32.subarray(argPtr >> 2);

		argView[0] = x;
		argView[1] = y;
		this.run(id, ApiTakeCode.PLACE_SINGLE_ROAD, argPtr);
		
		this.module._free(argPtr);
	}
}



