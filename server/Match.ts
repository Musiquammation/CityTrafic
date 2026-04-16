import { Client } from "./Client";
import { shared } from "./shared";

export class Match {
	readonly id: number;
	clients: Client[] = [];
	mapX: number;
	mapY: number;
	mapW: number;
	mapH: number;

	constructor(id: number,
		mapX: number,
		mapY: number,
		mapW: number,
		mapH: number
	) {
		this.id = id
		this.mapX = mapX;
		this.mapY = mapY;
		this.mapW = mapW;
		this.mapH = mapH;
	}

	sendUpdatedBlocks() {
		return Promise.all(this.clients.map(
			client => client.sendUpdatedBlocks()));
	}

	pushClient(client: Client) {
		this.clients.push(client);
		shared.pushClient(this.id);
	} 

	popClient(client: Client) {
		const idx = this.clients.indexOf(client);
		this.clients.splice(idx, 1);
		shared.popClient(this.id, idx);
	}
}
