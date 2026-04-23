import { COMMAND_CODES } from "../shared/CommandCode";
import { sendCommand } from "../net/sendCommand";
import { Action } from "./Action";
import { ActionConstructor } from "./ActionConstructor";





export class RoadAction extends Action {
	static readonly CONSTRUCTOR = new class extends ActionConstructor {
		override construct(): Action {
			return new RoadAction();
		}
	}


	constructor() {
		super();
	}


	override select() {
		
	}

	override deselect() {
		
	}

	
	override mouseUp(x: number, y: number, btn: number) {

	}

	override mouseDown(x: number, y: number, btn: number) {
		if (btn === Action.LEFT_BTN) {
			sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
				writer.writeInt32(Math.floor(x));
				writer.writeInt32(Math.floor(y));
			});
		}
	}

	override mouseMove(prevX: number, prevY: number, x: number, y: number, btn: number) {
		if (btn === Action.LEFT_BTN) {
			const xfloor = Math.floor(x);
			const yfloor = Math.floor(y);
			if (Math.floor(prevX) != xfloor || Math.floor(prevY) != yfloor) {
				sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
					writer.writeInt32(xfloor);
					writer.writeInt32(yfloor);
				});
			}
		}
	}


}