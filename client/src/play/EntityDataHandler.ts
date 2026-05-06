import { sendSocket } from "../net/sendSocket";
import { DataReader } from "../shared/DataReader";
import { DataWriter } from "../shared/DataWriter";
import { SERVER_IDS } from "../shared/ServerId";
import { Character } from "./Character";
import { PlayState } from "./PlayState";

type nature_t = 'character' | 'car';

interface Pair {
	nature: nature_t;
	ptr: bigint;
}


const natures: nature_t[] = [
	"character",
	"car"
];

const statusList = [
	"idle",
	"to home",
	"to work",
	"fill fuel",
	"eat"	
];

interface CharacterBubble {
	type: 'character';
	x: number;
	y: number;
	action: string;
	money: number;
	food: number;
}

interface CarBubble {
	type: 'car';
	x: number;
	y: number;
}


type bubble_t = CharacterBubble|CarBubble;



interface Print {
	label: string;
	type: 'string' | 'int' | 'float';
	value: any;
}

function print(
	list: Print[],
	ctx: OffscreenCanvasRenderingContext2D,
	startX: number,
	startY: number
	) {
	const lineHeight = 1;
	const padding = .5;
	const boxWidth = 10;
	const boxHeight = (list.length * lineHeight) + (padding * 2);

	const topY = startY - boxHeight;

	ctx.fillStyle = "rgba(0, 0, 0, 0.8)";
	ctx.fillRect(startX, topY, boxWidth, boxHeight);

	ctx.strokeStyle = "#ffffff";
	ctx.lineWidth = .1;
	ctx.strokeRect(startX, topY, boxWidth, boxHeight);

	ctx.font = (0.7 * lineHeight) + "px monospace";
	ctx.textAlign = "left";
	ctx.textBaseline = "top";

	list.forEach((item, index) => {
		let displayValue = item.value;

		// Formatage selon le type
		if (item.type === 'float' && typeof item.value === 'number') {
			displayValue = item.value.toFixed(3);
		} else if (item.type === 'int') {
			displayValue = Math.floor(item.value).toString();
		}

		const text = `${item.label}: ${displayValue}`;
		const textY = topY + padding + (index * lineHeight);


		ctx.fillStyle = "white"; // Vert style "debug"
		ctx.fillText(text, startX + padding, textY);
	});
}

export class EntityDataHandler {
	private current: Pair|null=null;
	private bubble: bubble_t|null=null;
	private readonly waiting: Pair[] = [];
	private readonly playState: PlayState;

	constructor(playState: PlayState) {
		this.playState = playState;
	}

	send(ptr: bigint, nature: nature_t) {
		this.waiting.push({ptr, nature});


		const writer = new DataWriter();
		switch (nature) {
		case 'character':
			writer.writeUint8(SERVER_IDS.GET_CHARACTER);
			break;

		case 'car':
			writer.writeUint8(SERVER_IDS.GET_CHARACTER);
			break;
		}

		writer.skip(7);
		writer.writeUint64(ptr);
		sendSocket(writer.toArrayBuffer());
	}

	recv(reader: DataReader) {
		const isValid = reader.readUint8();
		const nature = natures[reader.readUint16()];
		const ptr = reader.readUint64();


		// Remove from waiting list
		for (let i = 0; i < this.waiting.length; i++) {
			if (this.waiting[i].nature !== nature || this.waiting[i].ptr !== ptr)
				continue;

			// Remove character
			this.waiting.splice(i, 1);
			break;
		}


		if (!isValid) {
			return;
		}

		if (this.current && ptr === this.current.ptr) {
			const c = this.playState.characters.find(
				c => c.ptr === ptr
			);

			if (c) {
				this.createBubble(c, reader, 'character');
			}
		}

		for (let i = 0; i < this.waiting.length; i++) {
			
		}
	}

	ask(ptr: bigint, nature: nature_t) {
		if (
			this.current &&
			this.current.ptr === ptr &&
			this.current.nature === nature
		) {return;}

		// Add pair in waiting list
		let send = true;
		for (const i of this.waiting) {
			if (i.ptr === ptr && i.nature === nature) {
				send = false;
				break;
			}
		}

		if (send) {
			this.send(ptr, nature);
		}


		this.current = {ptr, nature};
	}

	createBubble(character: Character, reader: DataReader, nature: nature_t) {
		switch (nature) {
		case 'character':
		{
			const action = statusList[reader.readUint32()];
			const money = reader.readUint32();
			const food = reader.readFloat32();
			
			this.bubble = {
				type: 'character',
				x: character.x-1,
				y: character.y-1,
				action,
				money,
				food
			};
			break
		}
		}
	}


	drawBubble(ctx: OffscreenCanvasRenderingContext2D) {
		if (!this.bubble)
			return;

		console.log(this.bubble);
		

		switch (this.bubble.type) {
		case "character":
			print([
				{label: 'x', type: 'float', value: this.bubble.x},
				{label: 'y', type: 'float', value: this.bubble.y},
				{label: 'action', type: 'string', value: this.bubble.action},
				{label: 'money', type: 'int', value: this.bubble.money},
				{label: 'food', type: 'float', value: this.bubble.food},
			], ctx, this.bubble.x, this.bubble.y);
			break;
		}
	}

	unselect() {
		this.current = null;
		this.bubble = null;
	}
}