import { sendCommand } from '../../net/sendCommand';
import { COMMAND_CODES } from '../../shared/CommandCode';
import { HandPanel } from '../HandPanel';
import {HandButton, HandObject} from './handtypes'

function erase(x: number, y: number, btn: number, current: number|null) {
	if (btn === HandPanel.LEFT_BTN) {
		// Check if current is not empty
		if (current !== null && ((current & 0xf) != 0)) {
			sendCommand(COMMAND_CODES.ERASE, writer => {
				writer.writeInt32(Math.floor(x));
				writer.writeInt32(Math.floor(y));
			});
		}

	}
}

function placeRoad(x: number, y: number, btn: number, current: number|null) {
	if (btn === HandPanel.LEFT_BTN) {
		// Check if current is not a road
		if (current !== null && ((current & 0xf) != 1)) {
			sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
				writer.writeInt32(Math.floor(x));
				writer.writeInt32(Math.floor(y));
			});
		}
	}
}

export const handlist = {
	erase: new HandButton(
		"assets/icons/erase.png",

		// enable
		()=>{},

		// diseable
		()=>{},

		// mouseUp
		(x, y, btn, current) => {
			erase(x, y, btn, current);
		},

		// mouseDown
		(x, y, btn, current) => {
			erase(x, y, btn, current);
		},

		// mouseMove
		(prevX, prevY, x, y, btn, current) => {
			erase(x, y, btn, current);
		},
	),

	road: new HandButton(
		"assets/icons/road.png",

		// enable
		()=>{},

		// diseable
		()=>{},

		// mouseUp
		(x, y, btn, current) => {
			placeRoad(x, y, btn, current);
		},

		// mouseDown
		(x, y, btn, current) => {
			placeRoad(x, y, btn, current);
		},

		// mouseMove
		(prevX, prevY, x, y, btn, current) => {
			placeRoad(x, y, btn, current);
		},
	)
}