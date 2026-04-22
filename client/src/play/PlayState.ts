import { DataWriter } from "../../../commons/DataWriter";
import { SERVER_IDS } from "../../../commons/serverIds";
import { GAME_HEIGHT, GAME_WIDTH } from "../handler/dimensions";
import { GameHandler } from "../handler/GameHandler";
import { InputHandler } from "../handler/InputHandler";
import { DrawStateData, GameState } from "../handler/states";
import { Chunk } from "../worker/Chunk";
import { sendSocket } from "../net/sendSocket";
import { sendCommand } from "../net/sendCommand";
import { drawCell } from "./drawCell";
import { MouseHandler } from "./MouseHandler";
import { askWorker } from "../worker/askWorker";
import { CommandCode } from "../../../commons/CommandCode";
import { ActionHandler } from "../action/ActionHandler";
import { Car } from "./Car";
import { Character } from "./Character";
import { drawCar } from "./drawCar";
import { drawCharacter } from "./drawCharacter";



export class PlayState extends GameState {
	private camX = 0;
	private camY = 0;
	private camZ = 50;
	private frameCount = 0;
	private cameraTimeout = -1;
	private mouseHandler = new MouseHandler(this);
	private viewBox_x = 0;
	private viewBox_y = 0;
	private viewBox_w = 1;
	private viewBox_h = 1;


	actionHandler = new ActionHandler(this);
	cars: Car[] = [];
	characters: Character[] = [];

	constructor() {
		super();
	}



	
	enter(data: any, input: InputHandler): void {
		document.getElementById("gameView")?.classList.remove("hidden");

		input.onMouseUp = e => this.mouseHandler.onMouseUp(e);
		input.onMouseDown = e => this.mouseHandler.onMouseDown(e);
		input.onMouseMove = e => this.mouseHandler.onMouseMove(e);
		input.onScroll = e => this.mouseHandler.onScroll(e);
		input.onTouchStart = e => this.mouseHandler.onTouchStart(e);
		input.onTouchEnd = e => this.mouseHandler.onTouchEnd(e);
		input.onTouchMove = e => this.mouseHandler.onTouchMove(e);


		// Send request to load area
		this.updateCamera(this.camX, this.camY, this.camZ);
		this.sendAskEntities();

		(window as any).playState = this;

	}


	test() {
		sendCommand(CommandCode.TEST, writer => {
			writer.writeInt32(18);
			writer.writeInt32(32);
			writer.writeInt32(44);
			writer.writeInt32(-1098);
		});
	}


	handleInputs(input: InputHandler) {
		
	}

	frame(game: GameHandler): GameState | null {
		this.handleInputs(game.inputHandler);

		this.frameCount++;
		return null;
	}



	private async drawGrid(ctx: OffscreenCanvasRenderingContext2D) {
		const rangeW = GAME_WIDTH/this.camZ;
		const rangeH = GAME_HEIGHT/this.camZ;

		const chunks = await askWorker<{
			x: number,
			y: number,
			cells: Uint16Array
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
					const cell = cells[j++]
					if (cell === 0)
						continue;

					ctx.save();
					ctx.translate(x+dx, y+dy);
					drawCell(cell, ctx);
					ctx.restore();
				}
			}
		}
	}

	private drawCars(ctx: OffscreenCanvasRenderingContext2D) {
		for (const car of this.cars) {
			ctx.save();
			ctx.translate(car.x, car.y);
			drawCar(car, ctx);
			ctx.restore();
		}
	}

	private drawCharacters(ctx: OffscreenCanvasRenderingContext2D) {
		for (const character of this.characters) {
			ctx.save();
			ctx.translate(character.x, character.y);
			drawCharacter(character, ctx);
			ctx.restore();
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
		await askWorker<void>('updateCells', [
			this.viewBox_x,
			this.viewBox_y,
			this.viewBox_w,
			this.viewBox_h
		]);
		

		// Draw game
		args.followCamera();
		await this.drawGrid(args.ctx);
		this.drawCharacters(args.ctx);
		this.drawCars(args.ctx);
		args.unfollowCamera();
		

		
		
	}

	exit() {
		document.getElementById("gameView")?.classList.add("hidden");

		if (this.cameraTimeout >= 0)
			clearTimeout(this.cameraTimeout);

		(window as any).playState = null;
		askWorker<void>('shutdown', []);
	}



	sendCameraUpdate() {
		const writer = new DataWriter();
		writer.writeUint8(SERVER_IDS.LISTEN);
		writer.skip(3);

		writer.writeInt32(this.viewBox_x);
		writer.writeInt32(this.viewBox_y);
		writer.writeInt32(this.viewBox_w);
		writer.writeInt32(this.viewBox_h);


		sendSocket(writer.toArrayBuffer());
	}

	updateCamera(x: number, y: number, z: number) {
		this.camX = x;
		this.camY = y;
		this.camZ = z;


		const EXPAND = 1.1;


		const width = EXPAND*GAME_WIDTH/this.camZ;
		const height = EXPAND*GAME_HEIGHT/this.camZ;

		this.viewBox_x = Math.floor(this.camX - width/2);
		this.viewBox_y = Math.floor(this.camY - height/2);
		this.viewBox_w = Math.floor(width);
		this.viewBox_h = Math.floor(height);


		if (this.cameraTimeout < 0) {
			this.cameraTimeout = setTimeout(() => {
				this.sendCameraUpdate();
				this.cameraTimeout = -1;
			}, 1000);
		}
	}

	getCamera() {
		return {x: this.camX, y: this.camY, z: this.camZ};
	}



	placeBlock(x: number, y: number) {
		const buffer = new DataWriter();
        buffer.writeUint8(SERVER_IDS.PLACE_SINGLE_ROAD);
        buffer.writeInt32(x);
        buffer.writeInt32(y);
        sendSocket(buffer.toArrayBuffer());
	}

	sendAskEntities() {
		const writer = new DataWriter();
		writer.writeUint8(SERVER_IDS.GET_ENTITIES);
		sendSocket(writer.toArrayBuffer());
	}
}
