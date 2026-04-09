import { Match } from "./Match";

class Shared {
	apiLib: any;
	matchs = new Map<string, Match>;

	constructor(apiLib: any) {
		this.apiLib = apiLib;
	}

	createMatch() {
		const hash = generateHash();

		/// TODO: implement this function
		const match = new Match(-1);
		
		return {match, hash};
	}
}

let shared: null | Shared = null;

export function setShared(apiLib: any) {
	shared = new Shared(apiLib);
}

export function getShared() {
	if (!shared)
		throw new Error("Shared is missing");
	
	return shared;
}