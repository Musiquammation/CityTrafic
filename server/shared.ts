import { Match } from "./Match";
import { api } from "./MatchApi";



class Shared {
	matchs = new Map<string, Match>;

	constructor() {
		
	}

	createMatch() {
		const hash = generateHash();

		/// TODO: implement this function
		const match = new Match(api.createSession());
		
		return {match, hash};
	}
}

export const shared = new Shared();

