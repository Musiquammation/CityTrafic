import { DataWriter } from "../../../commons/DataWriter";
import { SERVER_IDS } from "../../../commons/serverIds";
import { GAME_HEIGHT, GAME_WIDTH } from "../handler/dimensions";
import { GameHandler } from "../handler/GameHandler";
import { InputHandler } from "../handler/InputHandler";
import { DrawStateData, GameState } from "../handler/states";
import { Vector3 } from "../handler/Vector3";
import { sendSocket } from "../net/sendSocket";
import { api, Chunk } from "../SessionApi";
import { drawCell } from "./drawCell";


export class PlayState extends GameState {
	private camX = 0;
	private camY = 0;
	private camZ = 20;
	private frameCount = 0;

	constructor() {
		super();
	}

	enter(data: any, input: InputHandler): void {
		api.createSession();

		input.onMouseUp = e => {};
		input.onMouseDown = e => {};
		input.onMouseMove = e => {};
		input.onScroll = e => {};
		input.onTouchStart = e => {};
		input.onTouchEnd = e => {};
		input.onTouchMove = e => {};


		this.test();
	}


	test() {
		console.log(this);	
	}

	frame(game: GameHandler): GameState | null {
		if ((this.frameCount % 10) === 0) {
			console.log("place", this.frameCount / 10);
			api.placeRoad(this.frameCount / 10, 2);
		}


		this.frameCount++;
		return null;
	}



	private drawGrid(ctx: CanvasRenderingContext2D) {
		const rangeW = Math.floor(GAME_WIDTH/this.camZ);
		const rangeH = Math.floor(GAME_HEIGHT/this.camZ);
		for (const {x, y, cells} of api.getChunks(this.camX, this.camY, rangeW, rangeH)) {
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

	draw(args: DrawStateData): void {
		const ctx = args.ctx;

		// Background
		{
			ctx.fillStyle = "#261f19";
			ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);
		}

		// Update cells
		api.updateCells(this.camX, this.camY);
		
		// Draw game
		args.followCamera();
		this.drawGrid(args.ctx);
		args.unfollowCamera();
		
		
	}

	exit() {
		api.deleteSession();   
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