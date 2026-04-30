import { GameHandler } from "../handler/GameHandler";
import { InputHandler } from "../handler/InputHandler";
import { DrawStateData, GameState } from "../handler/states";
import { Vector3 } from "../handler/Vector3";
import { sendSocket } from "../net/sendSocket";
import { DataWriter } from "../shared/DataWriter"
import { DataReader } from "../shared/DataReader"
import { SERVER_IDS } from "../shared/ServerId"
import { CLIENT_IDS } from "../shared/ClientId"


export class HomeState extends GameState {
	constructor() {
		super();
	}

	enter(data: any, input: InputHandler): void {
		document.getElementById("homeView")?.classList.remove("hidden");

		document.getElementById("home-createSession")?.
			addEventListener('click', () => this.createSession("0000000000000000"));

		document.getElementById("home-joinSession")?.
			addEventListener('click', () => {
				const code = prompt("Session");
				if (code !== null)
					this.joinSession(code, "0000000000000000");
			}
		);

		input.onMouseUp = e => {};
		input.onMouseDown = e => {};
		input.onMouseMove = e => {};
		input.onScroll = e => {};
		input.onTouchStart = e => {};
		input.onTouchEnd = e => {};
		input.onTouchMove = e => {};
	}

	frame(game: GameHandler): GameState | null {
		return null;
	}

	async draw(args: DrawStateData) {
		
	}

	exit() {
		document.getElementById("homeView")?.classList.add("hidden");	
	}

	getCamera(): Vector3 | null {
		return null;
	}

	createSession(playerHash: string) {
		const writer = new DataWriter();
		writer.writeUint8(SERVER_IDS.CONNECT);
		writer.skip(7);
		writer.write256("0000000000000000");
		writer.write256(playerHash);
		sendSocket(writer.toArrayBuffer());
	}
	
	joinSession(code: string, playerHash: string) {
		const writer = new DataWriter();
		writer.writeUint8(SERVER_IDS.CONNECT);
		writer.skip(7);
		writer.write256(code);
		writer.write256(playerHash);
		sendSocket(writer.toArrayBuffer());
	}
}


