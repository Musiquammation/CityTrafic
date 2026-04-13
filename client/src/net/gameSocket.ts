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

gameSocket.addEventListener("message", (event) => {
	
});

gameSocket.addEventListener("error", (err) => {
	console.error(err);
});

gameSocket.addEventListener("close", (event) => {
	console.log("Socket closed:", event.code, event.reason);
});

