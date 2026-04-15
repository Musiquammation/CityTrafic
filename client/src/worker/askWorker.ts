const worker = new Worker("./dist/worker.js", {
	type: "module",
});

worker.postMessage({
	method: 'init',
	requestId: -1,
	wasmPath: (window as any).WASM_PATH
});


type Pending = {
	resolve: (value: any) => void;
	reject: (reason?: any) => void;
};

const pendings = new Map<number, Pending>();
let nextRequestId = 0;

worker.onmessage = event => {
	const { requestId, result, error } = event.data;
	
	const pending = pendings.get(requestId);
	if (!pending) return;

	pendings.delete(requestId);

	if (error) pending.reject(error);
	else pending.resolve(result);
}

export async function askWorker<T>(method: string, args: any[], transfered: Transferable[] = []) {
	const requestId = nextRequestId++;
	const promise = new Promise((resolve, reject) => {
		pendings.set(requestId, { resolve, reject });
	});

	worker.postMessage({
		requestId,
		method,
		args
	}, transfered);

	return (await promise) as T;
}

export function postWorker(method: string, args: any[], transfered: Transferable[] = []) {
	worker.postMessage({
		requestId: -1,
		method,
		args
	}, transfered);
}