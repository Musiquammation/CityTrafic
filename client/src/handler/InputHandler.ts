import type { Control } from "./Control";

type Mode = "zqsd" | "wasd";

class Keydown {
	turnLeft = false;
	turnRight = false;
	yieldIns = false;
	light = false;
	fastView = false;
	altern = false;
}

enum Action {
	NONE,
	DOWN,
	UP,
	DOWN_THEN_UP,
	UP_THEN_DOWN,
};

class KeyboardCollector {
	turnLeft = Action.NONE;
	turnRight = Action.NONE;
	yieldIns = Action.NONE;
	light = Action.NONE;
	fastView = Action.NONE;
	altern = Action.NONE;
}



export class InputHandler {
	static CONTROLS: Control[] = ['turnLeft', 'turnRight', 'yieldIns', 'light', 'fastView', 'altern'];
	static CONTROL_STACK_SIZE = 256;


	private collectedKeys: Record<Control, Action> = new KeyboardCollector();

	private keysDown: Record<Control, boolean> = new Keydown();

	private firstPress: Record<Control, boolean> = new Keydown();

	private killedPress: Record<Control, boolean> = new Keydown();

	private keyMap: Record<string, Control>;



	onMouseUp = (e: MouseEvent) => {};
	onMouseDown = (e: MouseEvent) => {};
	onMouseMove = (e: MouseEvent) => {};
	onScroll = (e: WheelEvent) => {};
	onTouchStart = (e: TouchEvent) => {};
	onTouchEnd = (e: TouchEvent) => {};
	onTouchMove = (e: TouchEvent) => {};

	static KEYBOARDS: Record<Mode, Record<string, Control>> = {
		zqsd: {
			KeyE: 'turnLeft',
			KeyR: 'turnRight',
			KeyP: 'yieldIns',
			KeyL: 'light',
			KeyC: 'fastView',
			KeyS: 'altern',
		},

		wasd: {
			KeyE: 'turnLeft',
			KeyR: 'turnRight',
			KeyP: 'yieldIns',
			KeyL: 'light',
			KeyC: 'fastView',
			KeyS: 'altern',
		},
	};


	constructor(mode: Mode) {
		this.keyMap = InputHandler.KEYBOARDS[mode];
	}

	applyKeydown(control: Control) {
		switch (this.collectedKeys[control]) {
			case Action.NONE:
			this.collectedKeys[control] = Action.DOWN;
			break;
			
		case Action.DOWN:
			break;
			
		case Action.UP:
			this.collectedKeys[control] = Action.UP_THEN_DOWN;
			break;

		case Action.DOWN_THEN_UP:
			this.collectedKeys[control] = Action.UP_THEN_DOWN;
			break;

		case Action.UP_THEN_DOWN:
			this.collectedKeys[control] = Action.UP_THEN_DOWN;
			break;

		}
	}

	applyKeyup(control: Control) {
		switch (this.collectedKeys[control]) {
		case Action.NONE:
			this.collectedKeys[control] = Action.UP;
			break;
			
		case Action.DOWN:
			this.collectedKeys[control] = Action.DOWN_THEN_UP;
			break;
			
		case Action.UP:
			break;

		case Action.DOWN_THEN_UP:
			this.collectedKeys[control] = Action.DOWN_THEN_UP;
			break;

		case Action.UP_THEN_DOWN:
			this.collectedKeys[control] = Action.DOWN_THEN_UP;
			break;
			
		}
	}

	private onKeydown = (event: Event) => {
		const e = event as KeyboardEvent;
		const control = this.keyMap[e.code];
		if (control) {
			this.applyKeydown(control);
		}
	}

	private onKeyup = (event: Event) => {
		const e = event as KeyboardEvent;
		const control = this.keyMap[e.code];
		if (control) {
			this.applyKeyup(control);
			
		}
	}



	private onButtonTouchStart = (control: Control | 'special', element: HTMLElement) => {
		element.classList.add("high");

		if (control === 'special') {
			return;
		}

		switch (this.collectedKeys[control]) {
		case Action.NONE:
			this.collectedKeys[control] = Action.DOWN;
			break;
			
		case Action.DOWN:
			break;
			
		case Action.UP:
			this.collectedKeys[control] = Action.UP_THEN_DOWN;
			break;

		case Action.DOWN_THEN_UP:
			this.collectedKeys[control] = Action.UP_THEN_DOWN;
			break;

		case Action.UP_THEN_DOWN:
			this.collectedKeys[control] = Action.UP_THEN_DOWN;
			break;

		}
	}

	private onButtonTouchEnd = (control: Control | 'special', element: HTMLElement) => {
		element.classList.remove("high");

		if (control === 'special') {
			return;
		}

		switch (this.collectedKeys[control]) {
		case Action.NONE:
			this.collectedKeys[control] = Action.UP;
			break;
			
		case Action.DOWN:
			this.collectedKeys[control] = Action.DOWN_THEN_UP;
			break;
			
		case Action.UP:
			break;

		case Action.DOWN_THEN_UP:
			this.collectedKeys[control] = Action.DOWN_THEN_UP;
			break;

		case Action.UP_THEN_DOWN:
			this.collectedKeys[control] = Action.DOWN_THEN_UP;
			break;
			
		}
	}



	
	startKeydownListeners(target: EventTarget) {
		target.addEventListener("keydown", this.onKeydown);
		target.addEventListener("keyup", this.onKeyup);
	}

	startMouseListeners(target: EventTarget) {
		target.addEventListener('mouseup', e => {
			this.onMouseUp(e as MouseEvent);
		});

		target.addEventListener('mousedown', e => {
			this.onMouseDown(e as MouseEvent);
		});

		target.addEventListener('mousemove', e => {
			this.onMouseMove(e as MouseEvent);
		});

		target.addEventListener('wheel', e => {
			const we = e as WheelEvent;
			if (we.ctrlKey) {
				we.preventDefault();
			}

			this.onScroll(we);
		}, {passive: false});

		target.addEventListener('touchstart', e => {
			this.onTouchStart(e as TouchEvent);
		});

		target.addEventListener('touchend', e => {
			this.onTouchEnd(e as TouchEvent);
		});
		
		target.addEventListener('touchmove', e => {
			this.onTouchMove(e as TouchEvent);
		});
	}

	removeListeners(target: EventTarget) {
		target.removeEventListener("keydown", this.onKeydown);
		target.removeEventListener("keyup", this.onKeyup);
	}
	
	
	update() {
		for (const control of InputHandler.CONTROLS) {
			this.play(control, this.collectedKeys[control]);
			this.collectedKeys[control] = Action.NONE;
		}
	}

	play(control: Control, action: Action) {
		switch (action) {
		case Action.NONE:
			this.firstPress[control] = false;
			this.killedPress[control] = false;
			break;

		case Action.DOWN:
			if (this.keysDown[control]) {
				this.firstPress[control] = false;
			} else {
				this.firstPress[control] = true;
				this.keysDown[control] = true;
			}
			this.killedPress[control] = false;
			break;

		case Action.UP:
			if (this.keysDown[control]) {
				this.firstPress[control] = false;
				this.keysDown[control] = false;
				this.killedPress[control] = true;
			} else {
				this.firstPress[control] = false;
				this.killedPress[control] = false;
			}
			break;

		case Action.DOWN_THEN_UP:
			if (this.keysDown[control]) {
				this.firstPress[control] = false;
				this.keysDown[control] = false;
			} else {
				this.firstPress[control] = true;
			}
			this.killedPress[control] = true;

			
			break;

		case Action.UP_THEN_DOWN:
			if (this.keysDown[control]) {
				this.firstPress[control] = false;
				this.keysDown[control] = false;
				this.killedPress[control] = true;
			} else {
				this.firstPress[control] = false;
				this.killedPress[control] = false;
			}

			if (this.keysDown[control]) {
				this.firstPress[control] = false;
			} else {
				this.firstPress[control] = true;
				this.keysDown[control] = true;
			}
			this.killedPress[control] = false;
			break;
		}
	}

	press(control: Control): boolean {
		return this.firstPress[control] || this.keysDown[control];
	}

	first(control: Control): boolean {
		return this.firstPress[control];
	}

	killed(control: Control): boolean {
		return this.killedPress[control];
	}


	kill(control: Control, removeFirstPress = false) {
		this.keysDown[control] = false;

		if (removeFirstPress) {
			this.firstPress[control] = false;
		}
	}



	
}
