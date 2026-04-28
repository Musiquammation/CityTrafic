import { PlayState } from "../PlayState";

type icon_t = {list: {[key: string]: string;}, first: string};

export abstract class HandObject {
    abstract getIcons(): icon_t;
    abstract enable(): string|null;
    abstract diseable(): void;

    abstract mouseUp(x: number, y: number,
        btn: number, play: PlayState): void;

    abstract mouseDown(x: number, y: number,
        btn: number, play: PlayState): void;

    abstract mouseMove(prevX: number, prevY: number,
        x: number, y: number, btn: number, play: PlayState): void;

}

export class HandButton extends HandObject {
    readonly icons: icon_t;

    private readonly _enable: () => string|null;
    private readonly _disable: ()=>void;

    private readonly _mouseUp: (x: number, y: number,
        btn: number, play: PlayState)=>void;

    private readonly _mouseDown: (x: number, y: number,
        btn: number, play: PlayState)=>void;

    private readonly _mouseMove: (prevX: number, prevY: number,
        x: number, y: number, btn: number, play: PlayState)=>void;

    constructor(
        icons: icon_t,
        enable: ()=>string|null,
        disable: ()=>void,
        mouseUp: (x: number, y: number, btn: number, play: PlayState)=>void,
        mouseDown: (x: number, y: number, btn: number, play: PlayState)=>void = ()=>{},
        mouseMove: (prevX: number, prevY: number,
            x: number, y: number, btn: number, play: PlayState)=>void = ()=>{},
    ) {
        super();
        this.icons = icons;
        this._enable = enable;
        this._disable = disable;
        this._mouseUp = mouseUp;
        this._mouseDown = mouseDown;
        this._mouseMove = mouseMove;
    }

    getIcons() {
        return this.icons;
    }

    override enable() {
        return this._enable();
    }
    override diseable() {
        this._disable();
    }
    
    override mouseUp(x: number, y: number, btn: number, play: PlayState) {
        this._mouseUp(x, y, btn, play);
    }

    override mouseDown(x: number, y: number, btn: number, play: PlayState) {
        this._mouseDown(x, y, btn, play);
    }

    override mouseMove(prevX: number, prevY: number,
        x: number, y: number, btn: number, play: PlayState
    ) {
        this._mouseMove(prevX, prevY, x, y, btn, play);
    }

}


export class HandList extends HandObject {
    private selected = 0;
    private alive = false;
    readonly list: HandButton[];

    constructor(list: HandButton[]) {
        super();
        this.list = list;
    }

    override getIcons() {
        return this.list[0].getIcons();
    }

    override enable() {
        this.alive = true;
        return this.list[this.selected].enable();
    }
    
    override diseable() {
        this.alive = true;
        this.list[this.selected].diseable();        
    }
    
    override mouseUp(x: number, y: number, btn: number, play: PlayState) {
        this.list[this.selected].mouseUp(x, y, btn, play);
    }
    
    override mouseDown(x: number, y: number, btn: number, play: PlayState) {
        this.list[this.selected].mouseDown(x, y, btn, play);
    }

    override mouseMove(prevX: number, prevY: number,
        x: number, y: number, btn: number, play: PlayState
    ) {
        this.list[this.selected].mouseMove(prevX, prevY, x, y, btn, play);        
    }

    change(idx: number) {
        if (this.alive) {
            this.list[this.selected].diseable();
        }

        this.selected = idx;
        return this.list[idx].enable();
    }
}