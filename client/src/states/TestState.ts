import { GameHandler } from "../handler/GameHandler";
import { InputHandler } from "../handler/InputHandler";
import { DrawStateData, GameState } from "../handler/states";
import { Vector3 } from "../handler/Vector3";
import { api } from "../SessionApi";


export class TestState extends GameState {
    private camX = 0;
    private camY = 0;
    private camZ = 1;

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
        
    }

    frame(game: GameHandler): GameState | null {
        return null;
    }

    draw(args: DrawStateData): void {
        api.updateCells(this.camX, this.camY);

        console.log("frame");
        for (let i of api.getChunks(this.camX, this.camY, 1, 1)) {
            console.log(i.x, i.y);
        }
    }

    exit() {
        api.deleteSession();   
    }

    getCamera(): Vector3 | null {
        return null;
    }
}