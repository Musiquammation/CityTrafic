import createModule from "../api/api.js";

import { Vector } from "./tools/Vector";
import { Rectangle } from "./tools/Rectangle";


type Runner<T> = (arg: T) => void;

enum ApiTakeCode {
	TAKE_MAP_CPY,
	RLSE_MAP_CPY,

	TAKE_COORDS,
	RSLE_COORDS
};



class SessionApi {
	private module: any = null;
	private apiPtr: number = 0;
	private sessionId: number | null = null;

	async init() {
		this.module = await createModule();

		// Api_createApi(int threadnum)
		this.apiPtr = this.module._Api_createApi(1);
	}

	cleanup() {
		if (!this.module || !this.apiPtr) return;

		// Api_deleteSession(Api*, int id)
		if (this.sessionId !== null) {
			this.module._Api_deleteSession(this.apiPtr, this.sessionId);
			this.sessionId = null;
		}

		// Api_deleteApi(Api*)
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
		return this.module._Api_take(
			this.apiPtr,
			this.sessionId,
			code,
			args
		);
	}

	takeCoords(): Rectangle {
		const ptr = this.run(ApiTakeCode.TAKE_COORDS) >> 2;

		const x = this.module.HEAP32[ptr+0];
		const y = this.module.HEAP32[ptr+1];
		const w = this.module.HEAP32[ptr+2];
		const h = this.module.HEAP32[ptr+3];

		this.run(ApiTakeCode.RSLE_COORDS);

		return {x, y, w, h};
	}

	takeCells(rect: Rectangle, runner: Runner<Uint16Array>) {
		const arg = this.module._malloc(4 * 4); 
		this.module.HEAPU32[(arg>>2) + 0] = rect.x;
		this.module.HEAPU32[(arg>>2) + 1] = rect.y;
		this.module.HEAPU32[(arg>>2) + 2] = rect.w;
		this.module.HEAPU32[(arg>>2) + 3] = rect.h;

		const ptr = this.run(ApiTakeCode.TAKE_MAP_CPY, arg) >> 1;
		
		console.log(this.module);
		this.module._free(arg);

		const view = this.module.HEAPU16.subarray(
			ptr,
			ptr + (rect.w * rect.h)
		);

		runner(view);

		this.run(ApiTakeCode.RLSE_MAP_CPY);
	}
}

export const api = new SessionApi();
api.init();