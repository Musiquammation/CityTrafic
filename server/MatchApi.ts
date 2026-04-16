
import { ApiTakeCode } from "../commons/ApiTakeCode"



interface MatchObject {
	mapX: number;
	mapY: number;
	mapW: number;
	mapH: number;
}





// const FRAME_DELAY = 16;
const FRAME_DELAY = 1000;

export class MatchApi {
	private matchs = new Map<number, MatchObject>();
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

	freeBuffer() {
		this.run(-1, ApiTakeCode.FREE_BUFFER);
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

		return { x, y, w, h };
	}

	private createApiMatch(id: number): MatchObject {
		const rect = this.takeCoords(id);
		const argPtr = this.module._malloc(4 * 4);
		const argView = this.module.HEAPU32.subarray(argPtr >> 2);
		argView[0] = rect.x;
		argView[1] = rect.y;
		argView[2] = rect.w;
		argView[3] = rect.h;

		this.module._free(argPtr);
		
		return {
			mapX: rect.x,
			mapY: rect.y,
			mapW: rect.w,
			mapH: rect.h
		};
	}

	appendCellGrid(id: number) {
		const m = this.createApiMatch(id);
		this.matchs.set(id, m);
		return m;
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

		const gridPtr = this.run(id, ApiTakeCode.TAKE_MAP_PTR);
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

	takeMapEdits(
		id: number, x: number, y: number, 
		w: number, h: number, layer: number
	) {
		const argPtr = this.module._malloc(4 * 4);
		const argView = this.module.HEAP32.subarray(argPtr >> 2);
		argView[0] = x;
		argView[1] = y;
		argView[2] = w;
		argView[3] = h;
		argView[4] = layer;

		const ptr = this.run(id, ApiTakeCode.MAKE_MAP_EDITS, argPtr);
		
		this.module._free(argPtr);

		const length = this.module.HEAPU32[ptr>>2];

		// Copy map edits
		const array = new Uint32Array(this.module.HEAPU32.buffer, ptr, length);
		const result = new Uint32Array(length);
		result.set(array);
		console.log("takeMapEdits",array);


		return {
			transfered: [result.buffer],
			result
		};
	}

	placeSingleRoad(id: number, x: number, y: number) {
		const argPtr = this.module._malloc(4 * 2);
		const argView = this.module.HEAP32.subarray(argPtr >> 2);

		argView[0] = x;
		argView[1] = y;
		this.run(id, ApiTakeCode.PLACE_SINGLE_ROAD, argPtr);
		
		this.module._free(argPtr);
	}

	pushLayer(id: number) {
		this.run(id, ApiTakeCode.PUSH_LAYER);
	}

	popLayer(id: number, idx: number) {
		const argPtr = this.module._malloc(4 * 2);
		const argView = this.module.HEAP32.subarray(argPtr >> 2);

		argView[0] = idx;

		this.run(id, ApiTakeCode.POP_LAYER, argPtr);
		this.module._free(argPtr);

	}
}



