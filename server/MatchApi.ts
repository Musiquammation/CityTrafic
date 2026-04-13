import { createRequire } from 'module';

const require = createRequire(import.meta.url);

const addon = require('../game-bin/addon.node');


class MatchApi {
	private api;

	constructor() {
		this.api = new addon.ApiWrapper(4);
	}

	createSession(): number {
		return this.api.createSession();
	}
}


export const api = new MatchApi();

