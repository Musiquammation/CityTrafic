import { CLIENT_IDS } from "../../../commons/clientIds";
import { DataReader } from "../../../commons/DataReader";
import { DataWriter } from "../../../commons/DataWriter";
import { SERVER_IDS } from "../../../commons/serverIds";
import { getGameHandler } from "../gameHandler"
import { Car } from "../play/Car";
import { Character } from "../play/Character";
import { PlayState } from "../play/PlayState";
import { askWorker, postWorker } from "../worker/askWorker";
import { sendSocket } from "./sendSocket";

let REGION_SIZE = 1;

function net_joinCreated(reader: DataReader) {
	REGION_SIZE = reader.readUint32();
	const hash = reader.read256();
	console.log(hash);

	getGameHandler().setState(new PlayState());
	return null;
}

function net_joinAlive(reader: DataReader) {
	REGION_SIZE = reader.readUint32();
	const hash = reader.read256();
	console.log(hash);

	getGameHandler().setState(new PlayState());
	return null;
}

function net_areas(reader: DataReader) {
	reader.readUint8(); // for 16bits padding
	const areasCount = reader.readInt32();

	for (let count = 0; count < areasCount; count++) {
		const x0 = reader.readUint32() * REGION_SIZE;
		const y0 = reader.readUint32() * REGION_SIZE;

		const list = new Uint16Array(REGION_SIZE * REGION_SIZE);
		for (let i = 0; i < REGION_SIZE * REGION_SIZE; i++)
			list[i] = reader.readUint16();

		postWorker('setArea', [x0, y0, REGION_SIZE, REGION_SIZE, list], [list.buffer]);
	}


	return null;
}

function net_edits(reader: DataReader) {
	reader.readUint8(); // for 16bits padding
	reader.readUint16(); // for 32bits padding
	const length = reader.readUint32();
	const array = reader.readUint32Array(length-1);
	postWorker('applyEdits', [array], [array.buffer]);

	return null;
}


let lastEntityAsk = -Infinity;
const ENTITY_ASK_COULDOWN = 1000;


async function net_getEntities(reader: DataReader) {
	reader.readUint8();  //  i8 padding
	reader.readUint16(); // i32 padding

	/// TODO: read entities
	const msgSize = reader.readUint32(); // msg size

	const prevOffset = reader.getOffset();
	const buffer = reader.readUint8Array(msgSize);

	await askWorker('readEntities', [buffer], [buffer.buffer]);

	reader.setOffset(prevOffset);

	// Read cars
	const carsCount = reader.readUint32();
	const cars = new Array<Car>(carsCount);
	for (let i = 0; i < carsCount; i++) {
		const x = reader.readInt32();
		const y = reader.readInt32();
		const step = reader.readInt32();
		const speed = reader.readInt32();
		const flag = reader.readInt32();

		const direction = flag & 0xff;
		const state = (flag>>8) & 0xff;

		cars[i] = {x,y,step,speed,direction,state};
	}

	// Read characters
	const charactersCount = reader.readUint32();
	const characters = new Array<Character>(charactersCount);
	for (let i = 0; i < charactersCount; i++) {
		const x = reader.readInt32();
		const y = reader.readInt32();
		characters[i] = {x,y};
	}


	const state = getGameHandler().getState();
	if (!(state instanceof PlayState))
		return;


	state.cars = cars;
	state.characters = characters;

	const now = Date.now();
	const delta = now - lastEntityAsk;

	if (delta >= ENTITY_ASK_COULDOWN) {
		lastEntityAsk = now;
		state.sendAskEntities();
	} else {
		const couldown = isFinite(lastEntityAsk) ?
			ENTITY_ASK_COULDOWN-delta:
			ENTITY_ASK_COULDOWN;

		setTimeout(() => {
			lastEntityAsk = Date.now();
			state.sendAskEntities();
		}, couldown);
	}

}



export function handleMessage(reader: DataReader): DataWriter | null {
	const action = reader.readUint8();

	switch (action) {
	case CLIENT_IDS.JOIN_CREATED:
		return net_joinCreated(reader);

	case CLIENT_IDS.JOIN_ALIVE:
		return net_joinAlive(reader);

	case CLIENT_IDS.AREAS:
		return net_areas(reader);

	case CLIENT_IDS.EDITS:
		return net_edits(reader);

	case CLIENT_IDS.GET_ENTITIES:
		net_getEntities(reader);
		return null;
		
	default:
		throw new Error("Unknown action " + action);
	}

}