import { GAME_HEIGHT, GAME_WIDTH } from "../handler/dimensions";
import { HandPanel } from "./HandPanel";
import { PlayState } from "./PlayState";

export class MouseHandler {
	private readonly playState: PlayState;
	private lastScreenX = NaN;
	private lastScreenY = NaN;
	private x = NaN;
	private y = NaN;

	constructor(playState: PlayState) {
		this.playState = playState;
	}

	onMouseUp(e: MouseEvent) {

	}

	onMouseDown(e: MouseEvent) {
		const leftDown = (e.buttons & 1) !== 0;
		const rightDown = (e.buttons & 2) !== 0;
		const middleDown = (e.buttons & 4) !== 0;

		const {x, y} = this.getMousePosition(e.clientX, e.clientY);

		const action = this.playState.handPanel.getButton();
		if (leftDown) {
			action.mouseDown(x, y, HandPanel.LEFT_BTN, this.playState);
		}

		if (rightDown) {
			action.mouseDown(x, y, HandPanel.RIGHT_BTN, this.playState);
		}


		this.x = x;
		this.y = y;
		this.lastScreenX = e.clientX;
		this.lastScreenY = e.clientY;

	}

	onMouseMove(e: MouseEvent) {
		const leftDown = (e.buttons & 1) !== 0;
		const rightDown = (e.buttons & 2) !== 0;
		const middleDown = (e.buttons & 4) !== 0;

		let pos = this.getMousePosition(e.clientX, e.clientY);

		// Move camera
		if (middleDown && isFinite(this.x) && isFinite(this.y)) {
			const dx = pos.x - this.x;
			const dy = pos.y - this.y;
			const camera = this.playState.getCamera();

			this.playState.updateCamera(
				camera.x - dx,
				camera.y - dy,
				camera.z
			);

			// Update position
			pos = this.getMousePosition(e.clientX, e.clientY);
		}


		const action = this.playState.handPanel.getButton();
		
		const prev = this.getMousePosition(this.lastScreenX, this.lastScreenY);
		if (leftDown) {
			action.mouseMove(prev.x, prev.y, pos.x, pos.y,
				HandPanel.LEFT_BTN, this.playState);
		}

		if (rightDown) {
			action.mouseMove(prev.x, prev.y, pos.x, pos.y,
				HandPanel.RIGHT_BTN, this.playState);
		}


		// Update positions
		this.lastScreenX = e.clientX;
		this.lastScreenY = e.clientY;

		this.x = pos.x;
		this.y = pos.y;

	}

	onScroll(e: WheelEvent) {
		const camera = this.playState.getCamera();
		this.playState.updateCamera(camera.x, camera.y,  camera.z * (1 - e.deltaY / 1000));
	}

	onTouchStart(e: TouchEvent) {

	}

	onTouchEnd(e: TouchEvent) {

	}

	onTouchMove(e: TouchEvent) {

	}




	getMouse() {
		if (isNaN(this.x))
			return null;

		return { x: this.x, y: this.y };
	}

	private getMousePosition(mouseX: number, mouseY: number) {
		const camera = this.playState.getCamera();

		const scaleX = innerWidth / GAME_WIDTH;
		const scaleY = innerHeight / GAME_HEIGHT;
		const scale = Math.min(scaleX, scaleY);

		const offsetX = (innerWidth - GAME_WIDTH * scale) / 2;
		const offsetY = (innerHeight - GAME_HEIGHT * scale) / 2;

		let x = mouseX - offsetX;
		let y = mouseY - offsetY;

		x /= scale;
		y /= scale;

		x -= GAME_WIDTH / 2;
		y -= GAME_HEIGHT / 2;

		x /= camera.z;
		y /= camera.z;

		x += camera.x;
		y += camera.y;

		return { x, y };
	}
}