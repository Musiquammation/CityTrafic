import { Client } from "./Client";
import { shared } from "./shared";

export class Match {
	readonly id: number;
	layer = -1;
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

	async pushClient(client: Client) {
		this.clients.push(client);
		const layer = await shared.pushClient(this.id);
		this.layer = layer;
		return layer;
	} 

	popClient(client: Client) {
		const idx = this.clients.indexOf(client);
		this.clients.splice(idx, 1);
		shared.popClient(this.id, this.layer);
	}
}
