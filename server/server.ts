import os from 'os';
import dotenv from 'dotenv';
import path from 'path';
import { fileURLToPath } from 'url';
import net from 'net';
import { createRequire } from 'module';
import { DataReader } from './DataReader';
import { Session } from './Session';

dotenv.config();


process.on('SIGINT', cleanApi);
process.on('SIGTERM', cleanApi);
process.on('exit', cleanApi);

let apiMustBeClean = true;

function cleanApi() {
	if (!apiMustBeClean)
		return;

	apiMustBeClean = false;
	console.log('Api deleted!');
	process.exit(0);
}

const server = net.createServer(socket => {
	const session = new Session();
	socket.on('data', data => {
		const reader = new DataReader(data.buffer);
		const writer = session.receive(reader);
		if (writer) {
			socket.write(Buffer.from(writer.toArrayBuffer()));
		}


	});
	socket.on('close', () => {});
	socket.on('error', (err) => { console.error(err); });
});

const PORT = Number(process.env.PORT) || 3000;
server.listen(PORT, () => {
	console.log(`Server listening on port ${PORT}`);
});