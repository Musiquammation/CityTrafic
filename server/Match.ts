import { Client } from "./Client";

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
}