import { DataWriter } from "../shared/DataWriter";
import { SERVER_IDS } from "../shared/ServerId"
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
import { COMMAND_CODES } from "../shared/CommandCode";
import { Car } from "./Car";
import { Character, CHARACTER_SIZE } from "./Character";
import { drawCar } from "./drawCar";
import { drawCharacter } from "./drawCharacter";
import { HandPanel } from "./HandPanel";
import { ImageLoader } from "../handler/ImageLoader";
import { loadAssets } from "./loadAssets";
import { evalCalendar } from "./evalCalendar";

function modulo(a: number, n: number) {
	return (a % n + n) % n;
}


const html_day = document.getElementById("gameDay")!;
const html_year = document.getElementById("gameYear")!;
const html_hour = document.getElementById("gameHour")!;
const html_money = document.getElementById("gameMoney")!;

export class PlayState extends GameState {
	private camX = 0;
	private camY = 0;
	private camZ = 50;
	private frameCount = 0;
	private cameraUpdates = 0;
	private mouseHandler = new MouseHandler(this);
	private viewBox_x = 0;
	private viewBox_y = 0;
	private viewBox_w = 1;
	private viewBox_h = 1;
	private chunks: {
		x: number;
		y: number;
		cells: Uint16Array<ArrayBufferLike>;
	}[] = [];
	
	readonly handPanel: HandPanel;
	cars: Car[] = [];
	characters: Character[] = [];

	constructor() {
		super();

		const handPanelDiv = document.getElementById("handPanel")!;
		this.handPanel = new HandPanel(handPanelDiv);
	}



	
	enter(data: any, input: InputHandler, imageLoader: ImageLoader) {
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


		// To send camera update
		this.updateCamera(0, 0, 50);

		// Load
		this.handPanel.init(imageLoader);
		loadAssets(imageLoader);

		// For debug
		(window as any).playState = this;
		this.updateCamera(10, 10, 50);
	}


	test() {
		sendCommand(COMMAND_CODES.TEST, writer => {
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


	setCalendar(n: bigint) {
		const obj = evalCalendar(n);
		html_day.textContent = obj.day;
		html_year.textContent = obj.year;
		html_hour.textContent = obj.hour;

	}

	setMoney(money: number) {
		html_money.textContent = money + "$";
	}



	private async drawGrid(
		ctx: OffscreenCanvasRenderingContext2D,
		loader: ImageLoader
	) {
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


		// Update chunks
		this.chunks = chunks;

		for (const {x, y, cells} of chunks) {
			let j = 0;
			for (let dy = 0; dy < Chunk.SIZE; dy++) {
				for (let dx = 0; dx < Chunk.SIZE; dx++) {
					const cell = cells[j++]
					if (cell === 0)
						continue;

					ctx.save();
					ctx.translate(x+dx, y+dy);
					drawCell(cell, ctx, loader);
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
			ctx.translate(
				character.x - CHARACTER_SIZE.x/2,
				character.y - CHARACTER_SIZE.y/2
			);
			drawCharacter(character, ctx);
			ctx.restore();
		}
	}


	async draw(args: DrawStateData) {
		const ctx = args.ctx;

		// Background
		{
			ctx.fillStyle = "#D9E0E6";
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
		await this.drawGrid(args.ctx, args.imageLoader);
		this.drawCharacters(args.ctx);
		this.drawCars(args.ctx);
		args.unfollowCamera();
		

		
		
	}

	exit() {
		if (this.handPanel.isInitialized()) {
			this.handPanel.cleanup();
		}

		document.getElementById("gameView")?.classList.add("hidden");


		(window as any).playState = null;
		askWorker<void>('shutdown', []);
	}



	sendCameraUpdate() {
		console.log("cam");
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


		if (this.cameraUpdates === 0) {
			this.sendCameraUpdate();
		}

		this.cameraUpdates++;
	}

	resetCameraUpdates() {
		if (this.cameraUpdates === 1) {
			this.cameraUpdates = 0; // update resolved
			return;
		}

		this.sendCameraUpdate();
		this.cameraUpdates = 1; // waiting
	}


	getCamera() {
		return {x: this.camX, y: this.camY, z: this.camZ};
	}

	getCell(x: number, y: number) {
		const cx = Math.floor(x / Chunk.SIZE);
		const cy = Math.floor(y / Chunk.SIZE);

		for (const chunk of this.chunks) {
			if (chunk.x === cx*Chunk.SIZE && chunk.y === cy*Chunk.SIZE) {
				const cellX =  modulo(x, Chunk.SIZE);
				const cellY = modulo(y, Chunk.SIZE);
				return chunk.cells[cellY*Chunk.SIZE + cellX];
			}
		}

		return null;
	}

	
	sendAskEntities() {
		const writer = new DataWriter();
		writer.writeUint8(SERVER_IDS.UPDATE);
		sendSocket(writer.toArrayBuffer());
	}
}
