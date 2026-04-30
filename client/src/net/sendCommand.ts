import { COMMAND_CODES } from "../shared/CommandCode";
import { DataWriter } from "../shared/DataWriter";
import { SERVER_IDS } from "../shared/ServerId";
import { askWorker, postWorker } from "../worker/askWorker";
import { sendSocket } from "./sendSocket";

type QueuedCommand = {
	command: COMMAND_CODES;
	fill: (writer: DataWriter) => void;
};

const queue: QueuedCommand[] = [];

let flushTimer: any = null;
const FLUSH_DELAY = 100;
const MAX_BATCH = 250;

function scheduleFlush() {
	if (flushTimer !== null) return;

	flushTimer = setTimeout(() => {
		flushTimer = null;
		flushQueue();
	}, FLUSH_DELAY);
}

function flushQueue() {
	if (queue.length === 0) return;

	const batchSize = Math.min(queue.length, MAX_BATCH);

	const writer = new DataWriter();
	writer.writeUint8(SERVER_IDS.GAME_COMMANDS);
	writer.writeUint8(batchSize);

	for (let i = 0; i < batchSize; i++) {
		const cmd = queue[i];

		const offset16 = writer.getOffset() % 2;
		if (offset16) {writer.skip(1);}

		writer.writeUint16(cmd.command);

		const offset32 = writer.getOffset() % 4;
		if (offset32) {writer.skip(4 - offset32);}

		cmd.fill(writer);
	}

	queue.splice(0, batchSize);

	sendSocket(writer.toArrayBuffer());

	if (queue.length > 0) {
		scheduleFlush();
	}
}


export function sendCommand(
	command: COMMAND_CODES,
	fill: (writer: DataWriter) => void
) {
	const writer = new DataWriter();
	writer.writeUint16(command);
	writer.skip(2); // align game
	fill(writer);

	const buffer = writer.toArrayBuffer();

	// Run element
	postWorker('performGameCommand', [buffer], [buffer])



	// Add element in queue
	queue.push({ command, fill });

	if (queue.length >= MAX_BATCH) {
		if (flushTimer !== null) {
			clearTimeout(flushTimer);
			flushTimer = null;
		}
		flushQueue();
		return;
	}

	scheduleFlush();
}