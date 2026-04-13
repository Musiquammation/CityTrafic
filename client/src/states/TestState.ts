import { GameHandler } from "../handler/GameHandler";
import { InputHandler } from "../handler/InputHandler";
import { DrawStateData, GameState } from "../handler/states";
import { Vector3 } from "../handler/Vector3";
import { api } from "../SessionApi";


export class TestState extends GameState {
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
        const coords = api.takeCoords();
        console.log(coords);
    }

    frame(game: GameHandler): GameState | null {
        return null;
    }

    draw(args: DrawStateData): void {
        
    }

    exit() {
        api.deleteSession();   
    }

    getCamera(): Vector3 | null {
        return null;
    }
}