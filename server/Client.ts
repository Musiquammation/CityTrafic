import { DataReader } from "../commons/DataReader";
import { DataWriter } from "../commons/DataWriter";

import { CLIENT_IDS } from "../commons/clientIds"
import { SERVER_IDS } from "../commons/serverIds"
import { Match } from "./Match";
import { api } from "./MatchApi";
import { shared } from "./shared";




export class Client {
	static readonly MISSED_REGION_SIZE = 8;

	viewX = 0;
	viewY = 0;
	viewW = 0;
	viewH = 0;
	missedRegions = new Set<bigint>();
	visitedRegions = new Set<bigint>();
	match: Match | null = null;

	addMissedRegion(x: number, y: number) {
		const x0 = Math.floor(x/Client.MISSED_REGION_SIZE);
		const y0 = Math.floor(y/Client.MISSED_REGION_SIZE);

		const key = (BigInt(x0) << 32n) | BigInt(y0);
		if (!this.visitedRegions.has(key))
			return;


		const inside =
			x >= this.viewX &&
			x < this.viewX + this.viewW &&
			y >= this.viewY &&
			y < this.viewY + this.viewH;

		if (!inside) {
			this.missedRegions.add(key);
		}
	}

	receive(reader: DataReader): DataWriter | null {
		const action = reader.readUint8();

		switch (action) {
		case SERVER_IDS.CONNECT:
			return this.receive_connect(reader);

		case SERVER_IDS.LISTEN:
			return this.receive_listen(reader);

		default:
			throw new Error("Unknown action " + action);
		}
	}


	private receive_connect(reader: DataReader) {
		const matchHash = reader.read256();

		if (matchHash === "0000000000000000") {
			const {match, hash} = shared.createMatch();
			this.match = match;
			match.clients.push(this);

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

		match.clients.push(this);
		this.match = match;

		const writer = new DataWriter();
		writer.writeUint8(CLIENT_IDS.JOIN_ALIVE);
		writer.writeUint32(Client.MISSED_REGION_SIZE);
		writer.write256(matchHash);
		return writer;
	}

	private receive_listen(reader: DataReader) {
		if (!this.match)
			throw new Error("No match to listen");

		const match = this.match;
		if (!match.grid)
			throw new Error("Missing grid");

		const sendArea = (x: number, y: number) => {
			const area = api.collectArea(
				match,
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

		// Compute region bounds
		const rx0 = Math.floor(x / Client.MISSED_REGION_SIZE);
		const ry0 = Math.floor(y / Client.MISSED_REGION_SIZE);
		const rx1 = Math.floor((x + w - 1) / Client.MISSED_REGION_SIZE);
		const ry1 = Math.floor((y + h - 1) / Client.MISSED_REGION_SIZE);

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
		for (const key of this.missedRegions) {
			const rx = Number(key >> 32n);
			const ry = Number(key & 0xFFFFFFFFn);
			missedInView.push({ key, rx, ry });
		}

		// Count = new regions + missed regions
		const totalCount = newRegions.length + missedInView.length;
		writer.writeUint32(totalCount);

		// Send new (unvisited) regions
		for (const { key, rx, ry } of newRegions) {
			sendArea(rx, ry);
			this.visitedRegions.add(key);
		}

		// Send missed regions (and remove them from the set)
		for (const { key, rx, ry } of missedInView) {
			sendArea(rx, ry);
			this.missedRegions.delete(key);
		}

		// Expand visited regions to cover the full new view
		for (let ry = ry0; ry <= ry1; ry++) {
			for (let rx = rx0; rx <= rx1; rx++) {
				const key = (BigInt(rx) << 32n) | BigInt(ry);
				this.visitedRegions.add(key);
			}
		}

		// Update view
		this.viewX = x;
		this.viewY = y;
		this.viewW = w;
		this.viewH = h;

		

		return writer;
	}
}