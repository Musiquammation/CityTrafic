import { sendCommand } from '../../net/sendCommand';
import { COMMAND_CODES } from '../../shared/CommandCode';
import { HandPanel } from '../HandPanel';
import { PlayState } from '../PlayState';
import { HandButton } from './handtypes'
import { turnSelector } from './turnSelector'

function erase(x: number, y: number, btn: number, play: PlayState) {
	x = Math.floor(x);
	y = Math.floor(y);
	const current = play.getCell(x, y);

	if (btn === HandPanel.LEFT_BTN) {
		// Check if current is not empty
		if (current !== null && ((current & 0xf) != 0)) {
			sendCommand(COMMAND_CODES.ERASE, writer => {
				writer.writeInt32(x);
				writer.writeInt32(y);
			});
		}
	}
}

function placeRoad(x: number, y: number, btn: number, play: PlayState) {
	x = Math.floor(x);
	y = Math.floor(y);
	const current = play.getCell(x, y);
	
	if (btn === HandPanel.LEFT_BTN) {
		// Check if current is not a road
		if (current !== null && ((current & 0xf) != 1)) {
			sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
				writer.writeInt32(x);
				writer.writeInt32(y);
			});
		}
	}
}

function placeParking(x: number, y: number, btn: number, play: PlayState) {
	x = Math.floor(x);
	y = Math.floor(y);
	const current = play.getCell(x, y);
	
	if (btn === HandPanel.LEFT_BTN) {
		// Check if current is not a parking
		if (current !== null && ((current & 0xf) != 4)) {
			sendCommand(COMMAND_CODES.PARKING, writer => {
				writer.writeInt32(x);
				writer.writeInt32(y);
			});
		}
	}
}


function applyTurn(x: number, y: number, btn: number, play: PlayState, edit: boolean) {
	x = Math.floor(x);
	y = Math.floor(y);
	const current = play.getCell(x, y);

	// Place turn pixel
	if (btn === HandPanel.LEFT_BTN) {
		sendCommand(COMMAND_CODES.DIRECTION, writer => {
			writer.writeInt32(x);
			writer.writeInt32(y);
			writer.writeUint16(0);
		});

	} else if (btn === HandPanel.RIGHT_BTN && edit) {
		if (current === null || ((current & 0xf) != 5)) {
			return; // nothing to do
		}

		turnSelector.take(current, next => {
			if (next === null)
				return;

			sendCommand(COMMAND_CODES.DIRECTION, writer => {
				writer.writeInt32(x);
				writer.writeInt32(y);
				writer.writeUint16(next);
			});
		})
	}
}


export const handlist = {
	erase: new HandButton(
		{
			list: {erase: "assets/icons/erase.png"},
			first: 'erase'
		},

		// enable
		()=>null,

		// diseable
		()=>{},

		// mouseUp
		(x, y, btn, play) => {
			erase(x, y, btn, play);
		},

		// mouseDown
		(x, y, btn, play) => {
			erase(x, y, btn, play);
		},

		// mouseMove
		(prevX, prevY, x, y, btn, play) => {
			erase(x, y, btn, play);
		},
	),

	road: new HandButton(
		{
			list: {road: "assets/icons/road.png"},
			first: 'road'
		},

		// enable
		()=>null,

		// diseable
		()=>{},

		// mouseUp
		(x, y, btn, play) => {
			placeRoad(x, y, btn, play);
		},

		// mouseDown
		(x, y, btn, play) => {
			placeRoad(x, y, btn, play);
		},

		// mouseMove
		(prevX, prevY, x, y, btn, play) => {
			placeRoad(x, y, btn, play);
		},
	),


	parking: new HandButton(
		{
			list: {parking: "assets/icons/parking.png"},
			first: 'parking'
		},

		// enable
		()=>null,

		// diseable
		()=>{},

		// mouseUp
		(x, y, btn, play) => {
			placeParking(x, y, btn, play);
		},

		// mouseDown
		(x, y, btn, play) => {
			placeParking(x, y, btn, play);
		},

		// mouseMove
		(prevX, prevY, x, y, btn, play) => {
			placeParking(x, y, btn, play);
		},
	),

	turn: new HandButton(
		{
			list: {rotate: "assets/icons/rotate.png"},
			first: 'rotate'
		},

		// enable
		()=>null,

		// diseable
		()=>{},

		// mouseUp
		(x, y, btn, play) => {
			applyTurn(x, y, btn, play, false);
		},

		// mouseDown
		(x, y, btn, play) => {
			applyTurn(x, y, btn, play, true);
		},

		// mouseMove
		(prevX, prevY, x, y, btn, play) => {
			applyTurn(x, y, btn, play, false);
		},
	)

}