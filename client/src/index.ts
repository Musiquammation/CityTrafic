import { GameHandler } from "./handler/GameHandler";
import { setGameHandler } from "./gameHandler";

declare global {
	interface Window {
		game: any;
		running: any;
		startGame: any;
		useRequestAnimationFrame: boolean;
		DEBUG: boolean;
	}
}

window.game = null;
window.running = false;
window.useRequestAnimationFrame = true;
window.DEBUG = false;
window.startGame = startGame;


export function startGame() {
	const FPS_FREQUENCY = 4;
	const EXCESS_COUNT = 70;
	const EXCESS_LIM = 4 * FPS_FREQUENCY;

	let countedFps = 0;
	let excessCount = 0;

	setInterval(() => {
		const e = document.getElementById("fps");
		const count = countedFps * FPS_FREQUENCY;

		if (excessCount >= 0) {
			if (count > EXCESS_COUNT) {
				excessCount++;
				if (excessCount >= EXCESS_LIM) {
					window.useRequestAnimationFrame = false;
					excessCount = -1;
				}
			} else {
				excessCount = 0;
			}
		}

		if (e) {
			let text = count + "fps";
			if (!window.useRequestAnimationFrame) {
				text += " (async)";
			}
			e.textContent = text;
		}
		countedFps = 0
	}, 1000/FPS_FREQUENCY);

	

	const canvas = document.getElementById("gameCanvas") as HTMLCanvasElement;
	canvas.oncontextmenu = e => {
		e.preventDefault();
	};


	function resizeCanvas() {
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	
	resizeCanvas();
	window.addEventListener("resize", resizeCanvas);
	
	
	const keyboardMode = localStorage.getItem("keyboardMode");
	let realKeyboardMode: 'zqsd' | 'wasd';
	if (keyboardMode !== 'zqsd' && keyboardMode !== 'wasd') {
		realKeyboardMode = 'wasd'
	} else {
		realKeyboardMode = keyboardMode;
	}

	
	const canvasContext = canvas.getContext("2d")!;
	const game = new GameHandler(
		realKeyboardMode,
		canvas,
		document
	);

	setGameHandler(game);


	function runGameLoop() {
		game.gameLogic();
		game.gameDraw(
			canvasContext,
			canvas.width,
			canvas.height,
			(
				ctx: CanvasRenderingContext2D,
				followCamera: () => void,
				unfollowCamera: () => void
			) => {game.drawMethod(ctx, followCamera, unfollowCamera);}
		);
		
		if (window.running) {
			if (window.useRequestAnimationFrame) {
				requestAnimationFrame(runGameLoop);
			} else if (window.DEBUG) {
				setTimeout(runGameLoop, 1000/3);
			} else {
				setTimeout(runGameLoop, 1000/60);

			}
		}

		countedFps++;
	}

	// Share game object
	window.game = game;
	window.running = true;

	runGameLoop();
}




