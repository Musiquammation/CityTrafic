import { GameState, states } from "./states";
import {GAME_WIDTH, GAME_HEIGHT} from "./dimensions";
import { PauseElement } from "./PauseElement";
import {InputHandler} from "./InputHandler";
import { ImageLoader } from "./ImageLoader";
import { GAME_COLORS } from "./GAME_COLORS";
import { HomeState } from "../states/HomeState";

declare global {
	interface Window {
		IMG_ROOT_PATH: string;
	}
}

function setElementAsBackground(
	element: HTMLCanvasElement | HTMLImageElement,
	div: HTMLElement
): void {
	if (element instanceof HTMLCanvasElement) {
		element.toBlob(blob => {
			if (!blob) return;
			const url = URL.createObjectURL(blob);
			div.style.backgroundImage = `url(${url})`;
		});
	} else {
		div.style.backgroundImage = `url(${element.src})`;
	}
}

export class GameHandler {
	private state: GameState;

	inputHandler: InputHandler;
	imgLoader = new ImageLoader(window.IMG_ROOT_PATH);

	constructor(
		keyboardMode: "zqsd" | "wasd",
		mouseEventTarget: EventTarget,
		keydownEventTarget: EventTarget,
	) {
		this.inputHandler = new InputHandler(keyboardMode);
		this.inputHandler.startKeydownListeners(keydownEventTarget);
		this.inputHandler.startMouseListeners(mouseEventTarget);

		this.state = new HomeState();
		this.state.enter(undefined, this.inputHandler);

		/// TODO: Load assets
	}
	

	gameLogic() {
		this.inputHandler.update();
		const next = this.state.frame(this);

		if (next) {
			const data = this.state.exit();
			this.state = next;
			next.enter(data, this.inputHandler);
		}
	}

	gameDraw(ctx: CanvasRenderingContext2D, canvasWidth: number, canvasHeight: number, drawMethod: 
		(ctx: CanvasRenderingContext2D, followCamera: (()=>void), unfollowCamera: (()=>void)) => void
	) {
		const scaleX = canvasWidth / GAME_WIDTH;
		const scaleY = canvasHeight / GAME_HEIGHT;
		const scale = Math.min(scaleX, scaleY);
		const offsetX = (canvasWidth - GAME_WIDTH * scale) / 2;
		const offsetY = (canvasHeight - GAME_HEIGHT * scale) / 2;

		ctx.save();
		ctx.translate(offsetX, offsetY);
		ctx.scale(scale, scale);

		// Draw background
		const camera = this.state.getCamera();
		ctx.fillStyle = "black";
		ctx.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT);


		const followCamera = () => {
			ctx.save();
			if (camera) {
				ctx.translate(GAME_WIDTH / 2, GAME_HEIGHT / 2);
				ctx.scale(camera.z, camera.z);
				ctx.translate(-camera.x, -camera.y);
			}
		}

		const unfollowCamera = () => {
			ctx.restore();
		}
		
		drawMethod(ctx, followCamera, unfollowCamera);
		
		
		ctx.restore();

		
		
		// Make borders dark
		ctx.fillStyle = "black";
		if (offsetY > 0) ctx.fillRect(0, 0, canvasWidth, offsetY);
		if (offsetY > 0) ctx.fillRect(0, canvasHeight - offsetY, canvasWidth, offsetY);
		if (offsetX > 0) ctx.fillRect(0, 0, offsetX, canvasHeight);
		if (offsetX > 0) ctx.fillRect(canvasWidth - offsetX, 0, offsetX, canvasHeight);
	}
	
	drawMethod(ctx: CanvasRenderingContext2D, followCamera: (()=>void), unfollowCamera: (()=>void)) {
		this.state.draw({ctx, imageLoader: this.imgLoader, followCamera, unfollowCamera});
	}
}

