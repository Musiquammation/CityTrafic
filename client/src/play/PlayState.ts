import { DataWriter } from "../../../commons/DataWriter";
import { SERVER_IDS } from "../../../commons/serverIds";
import { GAME_HEIGHT, GAME_WIDTH } from "../handler/dimensions";
import { GameHandler } from "../handler/GameHandler";
import { InputHandler } from "../handler/InputHandler";
import { DrawStateData, GameState } from "../handler/states";
import { Vector3 } from "../handler/Vector3";
import { Chunk } from "../worker/Chunk";
import { sendSocket } from "../net/sendSocket";
import { drawCell } from "./drawCell";
import { askWorker } from "../worker/askWorker";



export class PlayState extends GameState {
	private camX = 0;
	private camY = 0;
	private camZ = 90;
	private frameCount = 0;
	private workerId = 0;

	private nextRequestId = 0;


	constructor() {
		super();
	}

	




	enter(data: any, input: InputHandler): void {
		input.onMouseUp = e => {};
		input.onMouseDown = e => {};
		input.onMouseMove = e => {};
		input.onScroll = e => {};
		input.onTouchStart = e => {};
		input.onTouchEnd = e => {};
		input.onTouchMove = e => {};


		// Send request to load area
		this.updateCamera(this.camX, this.camY, this.camZ);

		(window as any).playState = this;
		this.test();
	}


	test() {

	}

	frame(game: GameHandler): GameState | null {
		this.frameCount++;
		return null;
	}



	private async drawGrid(ctx: CanvasRenderingContext2D) {
		const rangeW = GAME_WIDTH/this.camZ;
		const rangeH = GAME_HEIGHT/this.camZ;

		const chunks = await askWorker<{
			x: number,
			y: number,
			cells: any
		}[]>('getChunks', [
			Math.floor(this.camX - rangeW/2),
			Math.floor(this.camY - rangeH/2),
			Math.floor(rangeW),
			Math.floor(rangeH),
		]);


		for (const {x, y, cells} of chunks) {
			let j = 0;
			for (let dy = 0; dy < Chunk.SIZE; dy++) {
				for (let dx = 0; dx < Chunk.SIZE; dx++) {
					ctx.save();
					ctx.translate(x+dx, y+dy);
					drawCell(cells[j++], ctx);
					ctx.restore();
				}
			}
		}
	}

	async draw(args: DrawStateData) {
		const ctx = args.ctx;

		// Background
		{
			ctx.fillStyle = "#261f19";
			ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);
		}

		// Update cells
		await askWorker<void>('updateCells', [this.camX, this.camY]);
		
		// Draw game
		args.followCamera();
		this.drawGrid(args.ctx);
		args.unfollowCamera();
		
		
	}

	exit() {
		(window as any).playState = null;
		askWorker<void>('shutdown', []);
	}

	getCamera(): Vector3 | null {
		return {x: this.camX, y: this.camY, z: this.camZ};
	}


	updateCamera(x: number, y: number, z: number) {
		const EXPAND = 1.5;
		const writer = new DataWriter();
		writer.writeUint8(SERVER_IDS.LISTEN);

		const width = EXPAND*GAME_WIDTH/z;
		const height = EXPAND*GAME_HEIGHT/z;
		
		writer.writeUint32(Math.floor(x - width/2));
		writer.writeUint32(Math.floor(y - height/2));
		writer.writeUint32(Math.floor(width));
		writer.writeUint32(Math.floor(height));



		sendSocket(writer.toArrayBuffer());
	}
}
