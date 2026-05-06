import { sendCommand } from "../net/sendCommand";
import { sendSocket } from "../net/sendSocket";
import { DataReader } from "../shared/DataReader";
import { DataWriter } from "../shared/DataWriter";
import { SERVER_IDS } from "../shared/ServerId";
import { DataPanel } from "./panel/DataPanel";
import { PANEL_MAP } from "./panel/PANEL_MAP";

const resolvers = new Map<
	number,
	(reader: DataReader) => void
>();

function wait(id: number) {
	return new Promise<DataReader>((resolve) => {
		resolvers.set(id, resolve);
	});
}

function randomUint16(): number {
	const arr = new Uint16Array(1);
	crypto.getRandomValues(arr);
	return arr[0];
}

type arg_t = {
	building?: {x: number, y: number},
	job?: {idx: number},
};

function appendArgs(writer: DataWriter, arg: arg_t) {
	if (arg.building) {
		writer.writeInt32(arg.building.x);
		writer.writeInt32(arg.building.y);
		return;
	}

	if (arg.job) {
		writer.writeInt32(-0x80000000);
		writer.writeInt32(arg.job.idx);
		return
	}

	throw new Error("Missing argument");
}

export async function runPanel(args: arg_t) {
	const panel = new DataPanel();
	panel.open();

	const requestWriter = new DataWriter();
	requestWriter.writeUint8(SERVER_IDS.PANEL);
	requestWriter.writeUint8(0); // read mode

	const requestId = randomUint16();
	requestWriter.writeUint16(requestId);

	appendArgs(requestWriter, args);

	sendSocket(requestWriter.toArrayBuffer());

	const reader = await wait(requestId);

	const panelId = reader.readUint32();
	const descriptor = PANEL_MAP.get(panelId)!;

	const element = document.createElement("div");
	// Build element
	for (let field of descriptor.fields) {
		const value = field.unit.read(reader);
		const e = field.unit.create(field.name, value);
		element.appendChild(e);
	}


	// Collect
	const confirm = await new Promise<boolean>(resolve => {
		panel.append(element, (confirm) => {
			resolve(confirm);
		});
	});


	if (!confirm) {
		throw new Error("Panel edition cancelled");
	}

	const writer = new DataWriter();
	writer.writeUint8(SERVER_IDS.PANEL); // write mode
	writer.writeUint8(1); // write mode
	writer.skip(2);
	appendArgs(writer, args);

	for (let i = 0; i < descriptor.fields.length; i++) {
		const u = descriptor.fields[i].unit;
		u.write(
			writer,
			u.finish(element.children[i] as HTMLElement)
		);
	}

	sendSocket(writer.toArrayBuffer());

}

export function resolvePanel(id: number, reader: DataReader) {
	const resolve = resolvers.get(id);
	if (!resolve) return;

	resolve(reader);
	resolvers.delete(id);
}