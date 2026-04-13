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


	private run(code: number) {
		return this.module._Api_take(
			this.apiPtr,
			this.sessionId,
			code
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

	takeCells(runner: Runner<void>) {
		const ptr = this.run(ApiTakeCode.TAKE_MAP_CPY);

		runner();
		this.run(ApiTakeCode.RLSE_MAP_CPY);

	}
}

export const api = new SessionApi();
api.init();