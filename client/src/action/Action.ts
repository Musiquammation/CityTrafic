export abstract class Action {
	static readonly LEFT_BTN   = 0;
	static readonly RIGHT_BTN  = 1;
	static readonly MIDDLE_BTN = 2;

	abstract select(): void;
	abstract deselect(): void;

	abstract mouseUp(x: number, y: number, btn: number): void;
	abstract mouseDown(x: number, y: number, btn: number): void;
	abstract mouseMove(prevX: number, prevY: number,
		x: number, y: number, btn: number): void;
}