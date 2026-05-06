import { sendCommand } from '../../net/sendCommand';
import { sendSocket } from '../../net/sendSocket';
import { COMMAND_CODES } from '../../shared/CommandCode';
import { HandPanel } from '../HandPanel';
import { PlayState } from '../PlayState';
import { runPanel } from '../runPanel';
import { HandButton } from './handtypes'
import { turnSelector } from './turnSelector'

function applyDefaultRightClick(
	x: number, y: number, current: number, play: PlayState
) {
	const cellType = (current & 0xf);
	console.log(cellType);


	// Turn direction
	if (cellType === 5) {
		turnSelector.take(current, next => {
			if (next === null)
				return;

			sendCommand(COMMAND_CODES.DIRECTION, writer => {
				writer.writeInt32(x);
				writer.writeInt32(y);
				writer.writeUint16(next);
			});
		})

		return;
	}

	// Building
	if (cellType === 2 || cellType === 3) {
		runPanel({building: {x, y}});
		return;
	}
}

function erase(
	x: number, y: number, btn: number,
	play: PlayState, edit: boolean
) {
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

		return;
	}

	if (btn === HandPanel.RIGHT_BTN && edit && current !== null) {
		applyDefaultRightClick(x, y, current, play);
		return;
	}

}

function placeRoad(
	x: number, y: number, btn: number,
	play: PlayState, edit: boolean
) {
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
		
		return;
	}

	if (btn === HandPanel.RIGHT_BTN && edit && current !== null) {
		applyDefaultRightClick(x, y, current, play);
		return;
	}

}

function placeParking(
	x: number, y: number, btn: number,
	play: PlayState, edit: boolean
) {
	x = Math.floor(x);
	y = Math.floor(y);
	const current = play.getCell(x, y);
	
	if (btn === HandPanel.LEFT_BTN) {
		// Check if current is not a parking
		if (current === null || ((current & 0xf) === 4))
			return;

		sendCommand(COMMAND_CODES.PARKING, writer => {
			writer.writeInt32(x);
			writer.writeInt32(y);
		});

		return;
	}

	if (btn === HandPanel.RIGHT_BTN && edit && current !== null) {
		applyDefaultRightClick(x, y, current, play);
		return;
	}

}


function applyTurn(
	x: number, y: number, btn: number,
	play: PlayState, edit: boolean
) {
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

		return;

	}
	
	if (btn === HandPanel.RIGHT_BTN && edit && current !== null) {
		applyDefaultRightClick(x, y, current, play);
		return;
	}
}





function ask(play: PlayState, label: string, text: string) {
	const r = prompt(text);
	if (r === null)
		play.handData = null;
	
	if (play.handData) {
		play.handData[label] = r;
	}
}




const placeHome = new HandButton(
	{
		list: {home: "assets/home.png"},
		first: 'home'
	},

	// enable
	play => {
		play.handData = {};

		
		
		ask(play, 'money',"Total money given to employees " +
				"(to motivate citizen to get this job)");

		ask(play, 'capacity',
				"Capacity (increase construction time)");
		ask(play, 'rent', "Rent");

		return 'placeHome';
	},

	// diseable
	play => {
		play.handData = null;
	},

	// mouseUp
	(x, y, btn, play) => {
		

	},

	// mouseDown
	(x, y, btn, play) => {
		x = Math.floor(x);
		y = Math.floor(y);

		const current = play.getCell(x, y);
		if (btn === HandPanel.RIGHT_BTN && current !== null) {
			applyDefaultRightClick(x, y, current, play);
		}
		
		const data = play.handData;
		if (data === null)
			return;

		sendCommand(COMMAND_CODES.PLACE_HOME, writer => {
			writer.writeUint32(x);
			writer.writeUint32(y);
			writer.writeInt32(data.money);
			writer.writeInt32(data.capacity);
			writer.writeInt32(data.rent);
		});
	},

	// mouseMove
	(prevX, prevY, x, y, btn, play) => {
	},
);

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
			erase(x, y, btn, play, false);
		},

		// mouseDown
		(x, y, btn, play) => {
			erase(x, y, btn, play, true);
		},

		// mouseMove
		(prevX, prevY, x, y, btn, play) => {
			erase(x, y, btn, play, false);
		},
	),

	road: new HandButton(
		{
			list: {road: "assets/icons/road.png"},
			first: 'road'
		},

		// enable
		()=>'road',

		// diseable
		()=>{},

		// mouseUp
		(x, y, btn, play) => {
			placeRoad(x, y, btn, play, false);
		},

		// mouseDown
		(x, y, btn, play) => {
			placeRoad(x, y, btn, play, true);
		},

		// mouseMove
		(prevX, prevY, x, y, btn, play) => {
			placeRoad(x, y, btn, play, false);
		},
	),


	parking: new HandButton(
		{
			list: {parking: "assets/icons/parking.png"},
			first: 'parking'
		},

		// enable
		()=>'parking',

		// diseable
		()=>{},

		// mouseUp
		(x, y, btn, play) => {
			placeParking(x, y, btn, play, false);
		},

		// mouseDown
		(x, y, btn, play) => {
			placeParking(x, y, btn, play, true);
		},

		// mouseMove
		(prevX, prevY, x, y, btn, play) => {
			placeParking(x, y, btn, play, false);
		},
	),

	turn: new HandButton(
		{
			list: {rotate: "assets/icons/rotate.png"},
			first: 'rotate'
		},

		// enable
		()=>'turn',

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
	),


	placeHome

}