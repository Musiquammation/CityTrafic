import { DataReader } from "../commons/DataReader";
import { DataWriter } from "../commons/DataWriter";

import { CLIENT_IDS } from "../commons/clientIds"
import { SERVER_IDS } from "../commons/serverIds"
import { shared } from "./shared";

export class Session {
	receive(reader: DataReader): DataWriter | null {
		const action = reader.readUint8();

		switch (action) {
		case SERVER_IDS.CONNECT: // connect
			return this.receive_connect(reader);

		default:
			throw new Error("Unknown action " + action);
		}
	}


	private receive_connect(reader: DataReader) {
		const matchId = reader.read256();

		if (matchId === "0000000000000000") {
			const writer = new DataWriter();
			writer.writeUint8(CLIENT_IDS.JOIN_CREATED);
			writer.write256(shared.createMatch().hash)

			return writer;
		}


		const match = shared.matchs.get(matchId);
		if (!match) {
			const writer = new DataWriter();
			writer.writeUint8(CLIENT_IDS.ERROR);
			return writer;
		}

		const writer = new DataWriter();
		writer.writeUint8(CLIENT_IDS.JOIN_ALIVE);
		writer.write256(matchId);
		return writer;
	}
}