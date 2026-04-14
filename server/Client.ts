import { DataReader } from "../commons/DataReader";
import { DataWriter } from "../commons/DataWriter";

import { CLIENT_IDS } from "../commons/clientIds"
import { SERVER_IDS } from "../commons/serverIds"
import { shared } from "./shared";




export class Client {
	static readonly MISSED_REGION_SIZE = 16;

	viewX = 0;
	viewY = 0;
	viewW = 0;
	viewH = 0;
	missedRegions = new Set<bigint>();
	matchHash: string | null = null;

	addMissedRegion(x: number, y: number) {
		const inside =
			x >= this.viewX &&
			x < this.viewX + this.viewW &&
			y >= this.viewY &&
			y < this.viewY + this.viewH;

		if (!inside) {
			const key = (BigInt(x) << 32n) | BigInt(y);
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
			this.matchHash = hash;
			match.clients.push(this);
			
			const writer = new DataWriter();
			writer.writeUint8(CLIENT_IDS.JOIN_CREATED);
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
		this.matchHash = matchHash;

		const writer = new DataWriter();
		writer.writeUint8(CLIENT_IDS.JOIN_ALIVE);
		writer.write256(matchHash);
		return writer;
	}

	private receive_listen(reader: DataReader) {
		const x = reader.readInt32();
		const y = reader.readInt32();
		const w = reader.readInt32();
		const h = reader.readInt32();

		const writer = new DataWriter();
		
		// Count missed blocks
		let missedBlocksCount = 0;
		/// TODO: count missed blocks


		// Send missed blocks
		writer.writeUint32(missedBlocksCount);


		return null;
	}
}