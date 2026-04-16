import { GameHandler } from "./GameHandler";
import { ImageLoader } from "./ImageLoader";
import { InputHandler } from "./InputHandler";
import { Vector3 } from "./Vector3";

export interface DrawStateData {
	ctx: OffscreenCanvasRenderingContext2D;
	imageLoader: ImageLoader;
	followCamera: (()=>void);
	unfollowCamera: (()=>void);
}

export abstract class GameState {
	abstract enter(data: any, input: InputHandler): void;
	abstract frame(game: GameHandler): GameState | null;
	abstract draw(args: DrawStateData): Promise<void>;
	abstract exit(): any;
	abstract getCamera(): Vector3 | null;
}


export namespace states {
	export class Home extends GameState {
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
		
		async draw(args: DrawStateData) {
			
		}
	
		exit(): void {
			
		}

		getCamera(): Vector3 | null {
			return null;
		}
	}
}
