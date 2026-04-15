import { ClientApi } from "./ClientApi";



const api = new ClientApi();



self.onmessage = event => {
    const {requestId, method} = event.data;
    const args = event.data.args as any[];


    if (method === 'init') {
        api.init(event.data.wasmPath);
        return;
    }

    if (method === 'shutdown') {
        api.cleanup();
        self.close();
        return;
    }


	if (typeof api[method as keyof ClientApi] !== "function") {
		throw new Error("Method not found");
	}

    // @ts-ignore
	const result = api[method as keyof ClientApi](...args);
	
    if (result !== undefined && requestId >= 0) {
	    self.postMessage({ requestId, result });
    }
};

