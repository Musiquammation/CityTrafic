import { DataReader } from "../shared/DataReader"
import { DataWriter } from "../shared/DataWriter"
import { CLIENT_IDS } from "../shared/clientIds"
import { handleMessage } from "./handleMessage";

declare global {
	interface Window {
        SERV_ADDRESS: string;
    }
}

const gameSocket = new WebSocket(window.SERV_ADDRESS);

export function sendSocket(data: ArrayBuffer) {
    return gameSocket.send(data);
}



gameSocket.addEventListener("open", () => {
    console.log("Socket open");
});

gameSocket.addEventListener("message", async event => {
    const data = event.data;
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
    const writer = handleMessage(reader);
    if (writer) {
        sendSocket(writer.toArrayBuffer());
    }

});

gameSocket.addEventListener("error", (err) => {
	console.error(err);
});

gameSocket.addEventListener("close", (event) => {
	console.log("Socket closed:", event.code, event.reason);
});


