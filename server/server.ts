import dotenv from 'dotenv';
import { WebSocketServer } from 'ws';
import { DataReader } from '../commons/DataReader';
import { Session } from './Session';

dotenv.config();

const PORT = Number(process.env.PORT) || 3000;

const wss = new WebSocketServer({ port: PORT });

wss.on('connection', (socket) => {
	const session = new Session();

	socket.on('message', async data => {
		let buffer: ArrayBuffer;

		if (data instanceof ArrayBuffer) {
			buffer = data;
		} else if (data instanceof Blob) {
			buffer = await data.arrayBuffer();
		} else if (typeof data === "string") {
			buffer = new TextEncoder().encode(data).buffer;
		} else {
			throw new Error("Unsupported WebSocket message type");
		}

		const reader = new DataReader(buffer);
		const writer = session.receive(reader);

		if (writer) {
			socket.send(Buffer.from(writer.toArrayBuffer()));
		}
	});

	socket.on('close', () => {
		
	});

	socket.on('error', (err) => {
		console.error(err);
	});
});

console.log(`WebSocket server listening on port ${PORT}`);