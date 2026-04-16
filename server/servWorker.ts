import { parentPort, workerData } from "worker_threads";
import fs from 'fs'

// @ts-ignore
import createModule from "./wasm/api.js"
import { MatchApi } from "./MatchApi"

const wasmBinary = fs.readFileSync("server/wasm/api.wasm");


const module = await createModule({
	wasmBinary,
});

const api = new MatchApi(module, workerData.indexStart, workerData.indexSpacing);
console.log(`Started thread ${workerData.indexStart}/${workerData.indexSpacing}`);


parentPort!.on("message", async (msg) => {
	const { requestId, method } = msg;
	const args = msg.args as any[];

	if (typeof api[method as keyof MatchApi] !== "function") {
		throw new Error("Method not found");
	}


	const result = (api as any)[method](...args);
	api.freeBuffer();

	
	if (result === undefined) {
		parentPort!.postMessage({ requestId });	
	
	} else if (result.transfered && result.result) {
		parentPort!.postMessage({
			requestId,
			result: result.result
		}, result.transfered);	
	} else {
		parentPort!.postMessage({ requestId, result });	
	}


});

parentPort!.on("close", () => {
	api.delete();
});

