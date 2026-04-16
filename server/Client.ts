import { WebSocket } from "ws";
import { DataReader } from "../commons/DataReader";
import { DataWriter } from "../commons/DataWriter";

import { CLIENT_IDS } from "../commons/clientIds"
import { SERVER_IDS } from "../commons/serverIds"
import { Match } from "./Match";
import { shared } from "./shared";


function clamp(v: number, min: number, max: number) {
	if (v <= min)
		return min;

	if (v >= max)
		return max;

	return v;
}


export class Client {
	static readonly MISSED_REGION_SIZE = 8;

	private socket: WebSocket;
	private viewX = 0;
	private viewY = 0;
	private viewW = 0;
	private viewH = 0;
	private visitedRegions = new Set<bigint>();
	private match: Match | null = null;

	constructor(socket: WebSocket) {
		this.socket = socket;
	}


	async sendUpdatedBlocks() {
		const match = this.match;
		if (!match)
			throw new Error("No match to listen");

		const edits = await shared.takeMapEdits(match.id,
			this.viewX, this.viewY, this.viewW, this.viewH);

		const writer = new DataWriter();
		writer.writeUint8(CLIENT_IDS.EDITS);
		writer.writeUint8(0); // for 16bits padding
		writer.writeUint16(0); // for 32bits padding
		writer.addUint32Array(edits);

		this.socket.send(writer.toArrayBuffer());
	}


	async receive(reader: DataReader): Promise<DataWriter | null> {
		const action = reader.readUint8();

		switch (action) {
		case SERVER_IDS.CONNECT:
			return await this.receive_connect(reader);

		case SERVER_IDS.LISTEN:
			return await this.receive_listen(reader);

		case SERVER_IDS.PLACE_SINGLE_ROAD:
			return await this.receive_placeSingleRoad(reader);

		default:
			throw new Error("Unknown action " + action);
		}
	}


	private async receive_connect(reader: DataReader) {
		const matchHash = reader.read256();

		if (matchHash === "0000000000000000") {
			const {match, hash} = await shared.createMatch();
			this.match = match;
			// match.clients.push(this);
			match.pushClient(this);

			const writer = new DataWriter();
			writer.writeUint8(CLIENT_IDS.JOIN_CREATED);
			writer.writeUint32(Client.MISSED_REGION_SIZE);
			writer.write256(hash);
			return writer;
		}


		const match = shared.matchs.get(matchHash);
		if (!match) {
			const writer = new DataWriter();
			writer.writeUint8(CLIENT_IDS.ERROR);
			return writer;
		}

		match.pushClient(this);
		this.match = match;

		const writer = new DataWriter();
		writer.writeUint8(CLIENT_IDS.JOIN_ALIVE);
		writer.writeUint32(Client.MISSED_REGION_SIZE);
		writer.write256(matchHash);
		return writer;
	}

	private async receive_listen(reader: DataReader) {
		if (!this.match)
			throw new Error("No match to listen");

		const match = this.match;


		/// TODO: fill missedRegions
		const missedRegions = new Array<bigint>();

		const promises: Promise<void>[] = [];
		const sendArea = async (x: number, y: number) => {
			const area = await shared.collectArea(
				match.id,
				x * Client.MISSED_REGION_SIZE,
				y * Client.MISSED_REGION_SIZE,
				Client.MISSED_REGION_SIZE,
				Client.MISSED_REGION_SIZE
			);

			writer.writeInt32(x);
			writer.writeInt32(y);
			writer.addUint16Array(area);
		};


		const x = reader.readInt32();
		const y = reader.readInt32();
		const w = reader.readInt32();
		const h = reader.readInt32();

		const writer = new DataWriter();
		writer.writeUint8(CLIENT_IDS.AREAS);
		writer.writeUint8(0); // for 16bits padding

		// Compute region bounds
		const rx0 = Math.floor(clamp(
			x,
			match.mapX,
			match.mapX+match.mapW-1
		)/Client.MISSED_REGION_SIZE);

		const ry0 = Math.floor(clamp(
			x,
			match.mapY,
			match.mapY+match.mapH-1
		)/Client.MISSED_REGION_SIZE);
		
		const rx1 = Math.floor(clamp(
			(x + w - 1),
			match.mapX,
			match.mapX+match.mapW-1
		)/Client.MISSED_REGION_SIZE);

		const ry1 = Math.floor(clamp(
			(y + h - 1),
			match.mapY,
			match.mapY+match.mapH-1
		)/Client.MISSED_REGION_SIZE);

		
		// Collect new (unvisited) regions in the view
		const newRegions: Array<{ key: bigint, rx: number, ry: number }> = [];
		for (let ry = ry0; ry <= ry1; ry++) {
			for (let rx = rx0; rx <= rx1; rx++) {
				const key = (BigInt(rx) << 32n) | BigInt(ry);
				if (!this.visitedRegions.has(key)) {
					newRegions.push({ key, rx, ry });
				}
			}
		}

		// Collect missed regions that fall inside the new view
		const missedInView: Array<{ key: bigint, rx: number, ry: number }> = [];
		for (const key of missedRegions) {
			const rx = Number(key >> 32n);
			const ry = Number(key & 0xFFFFFFFFn);
			missedInView.push({ key, rx, ry });
		}

		// Count = new regions + missed regions
		const totalCount = newRegions.length + missedInView.length;
		writer.writeUint32(totalCount);

		// Send new (unvisited) regions
		for (const { key, rx, ry } of newRegions) {
			promises.push(sendArea(rx, ry));
			this.visitedRegions.add(key);
		}

		// Send missed regions (and remove them from the set)
		for (const { key, rx, ry } of missedInView) {
			promises.push(sendArea(rx, ry));
		}

		// Expand visited regions to cover the full new view
		for (let ry = ry0; ry <= ry1; ry++) {
			for (let rx = rx0; rx <= rx1; rx++) {
				const key = (BigInt(rx) << 32n) | BigInt(ry);
				this.visitedRegions.add(key);
			}
		}

		await Promise.all(promises); // wait for promises

		// Update view
		this.viewX = x;
		this.viewY = y;
		this.viewW = w;
		this.viewH = h;


		return writer;
	}

	private async receive_placeSingleRoad(reader: DataReader) {
		if (!this.match)
			throw new Error("No match to listen");


		// Place block into grid
		const x = reader.readInt32();
		const y = reader.readInt32();

		await shared.placeSingleRoad(this.match.id, x, y);

		this.match.sendUpdatedBlocks();

		return null;
	}

}