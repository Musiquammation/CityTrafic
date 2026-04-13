import os from 'os'

import { createRequire } from 'module';

const require = createRequire(import.meta.url);

const addon = require('../game-bin/addon.node');


class MatchApi {
	private api;

	constructor() {
		const numThreads = Math.max(1, os.cpus().length - 1);
		this.api = new addon.ApiWrapper(numThreads);
	}

	createSession(): number {
		return this.api.createSession();
	}

	deleteSession(id: number) {
		this.api.deleteSession(id);
	}
}


export const api = new MatchApi();

