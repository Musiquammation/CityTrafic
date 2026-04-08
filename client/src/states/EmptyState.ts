import { GameHandler } from "../handler/GameHandler";
import { InputHandler } from "../handler/InputHandler";
import { DrawStateData, GameState } from "../handler/states";
import { Vector3 } from "../handler/Vector3";

export class EmptyState extends GameState {
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
	}

	frame(game: GameHandler): GameState | null {
		return null;
	}

	draw(args: DrawStateData): void {
		
	}

	exit() {
		
	}

	getCamera(): Vector3 | null {
		return null;
	}
}