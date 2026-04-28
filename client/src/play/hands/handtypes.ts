export abstract class HandObject {
    abstract getIcon(): string;
    abstract enable(): void;
    abstract diseable(): void;

    abstract mouseUp(x: number, y: number,
        btn: number, current: number): void;

    abstract mouseDown(x: number, y: number,
        btn: number, current: number): void;

    abstract mouseMove(prevX: number, prevY: number,
        x: number, y: number, btn: number, current: number): void;

}

export class HandButton extends HandObject {
    private readonly icon: string;

    private readonly _enable: ()=>void;
    private readonly _disable: ()=>void;

    private readonly _mouseUp: (x: number, y: number,
        btn: number, current: number)=>void;

    private readonly _mouseDown: (x: number, y: number,
        btn: number, current: number)=>void;

    private readonly _mouseMove: (prevX: number, prevY: number,
        x: number, y: number, btn: number, current: number)=>void;

    constructor(
        icon: string,
        enable: ()=>void,
        disable: ()=>void,
        mouseUp: (x: number, y: number, btn: number, current: number)=>void,
        mouseDown: (x: number, y: number, btn: number, current: number)=>void = ()=>{},
        mouseMove: (prevX: number, prevY: number,
            x: number, y: number, btn: number, current: number)=>void = ()=>{},
    ) {
        super();
        this.icon = icon;
        this._enable = enable;
        this._disable = disable;
        this._mouseUp = mouseUp;
        this._mouseDown = mouseDown;
        this._mouseMove = mouseMove;
    }

    getIcon() {
        return this.icon;
    }

    override enable() {
        this._enable();
    }
    override diseable() {
        this._disable();
    }
    
    override mouseUp(x: number, y: number, btn: number, current: number) {
        this._mouseUp(x, y, btn, current);
    }

    override mouseDown(x: number, y: number, btn: number, current: number) {
        this._mouseDown(x, y, btn, current);
    }

    override mouseMove(prevX: number, prevY: number,
        x: number, y: number, btn: number, current: number
    ) {
        this._mouseMove(prevX, prevY, x, y, btn, current);
    }

}
