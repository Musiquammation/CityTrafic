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

