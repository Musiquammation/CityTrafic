import { ClientApi } from "./ClientApi";
import createModule from "../../wasm/api.js"


const api = new ClientApi();

const moduleReadyResolvers: (() => void)[] = [];



function initModule(wasmPath: string) {
	createModule({
		locateFile(path: string) {
			if (path.endsWith(".wasm")) {
				return new URL(wasmPath, import.meta.url).href;
			}
			return path;
		}
	}).then((module: any) => {
		api.setModule(module);
		console.log("wasm module has been loaded");

		for (let resolve of moduleReadyResolvers) {
			resolve();
		}

		moduleReadyResolvers.length = 0;
	});

}

self.onmessage = async event => {
	const {requestId, method} = event.data;
	const args = event.data.args as any[];


	if (method === 'init') {
		initModule(event.data.wasmPath);
		return;
	}

	if (method === 'shutdown') {
		api.cleanup();
		self.close();
		return;
	}



	if (api.isModuleMissing()) {
		// Wait for module
		await (new Promise<void>(resolve => {
			moduleReadyResolvers.push(resolve);
		}));
	}

	// @ts-ignore
	const result = (api as any)[method](...args);

	api.freeBuffer();

	if (result === undefined) {
		self.postMessage({ requestId });	
	
	} else if (result.transfered && result.result) {
		self.postMessage({
			requestId,
			result: result.result
		}, result.transfered);
	} else {
		self.postMessage({ requestId, result });	
	}
};

