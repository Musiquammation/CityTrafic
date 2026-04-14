import { Match } from "./Match";
import { api } from "./MatchApi";
import { generateHash } from "./generateHash"


class Shared {
	matchs = new Map<string, Match>;

	constructor() {
		
	}

	createMatch() {
		const hash = generateHash();

		/// TODO: implement this function
		console.log("create");
		const match = new Match(api.createSession());
		console.log(api.takeCoords(match.id));
		
		return {match, hash};
	}
}

export const shared = new Shared();

