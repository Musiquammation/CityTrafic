import os from 'os';
import dotenv from 'dotenv';
import path from 'path';
import { fileURLToPath } from 'url';
import net from 'net';
import { createRequire } from 'module';
import { DataReader } from './DataReader';
import { Session } from './Session';
import { setShared } from './shared';

dotenv.config();

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// Utiliser createRequire pour charger le .node
const require = createRequire(import.meta.url);
const libPath = path.resolve(__dirname, String(process.env.LIB_PATH));
let apiAlive = true;

const apiLib = require(libPath);

const numThreads = Math.max(1, os.cpus().length - 1);
apiLib.Api_create(numThreads);

process.on('SIGINT', cleanApi);
process.on('SIGTERM', cleanApi);
process.on('exit', cleanApi);

setShared(apiLib);

function cleanApi() {
	if (!apiAlive)
		return;

	apiAlive = false;
	apiLib.Api_delete();
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