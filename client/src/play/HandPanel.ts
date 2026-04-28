import { ImageLoader } from "../handler/ImageLoader";
import { sendCommand } from "../net/sendCommand";
import { COMMAND_CODES } from "../shared/CommandCode";
import { handlist } from "./hands/handlist";
import { HandButton, HandList, HandObject } from "./hands/handtypes";




function setElementAsBackground(
	element: HTMLCanvasElement | HTMLImageElement,
	div: HTMLElement
) {
	if (element instanceof HTMLCanvasElement) {
		element.toBlob(blob => {
			if (!blob) return;
			const url = URL.createObjectURL(blob);
			div.style.backgroundImage = `url(${url})`;
		});
	} else {
		div.style.backgroundImage = `url(${element.src})`;
	}
}



export class HandPanel {
	static readonly LEFT_BTN   = 0;
	static readonly RIGHT_BTN  = 1;
	static readonly MIDDLE_BTN = 2;

	private div: HTMLElement;
	private initialized = 0;
	private selected = 0;
	private list: HandObject[] = [];

	constructor(div: HTMLElement) {
		this.div = div;
	}


	isInitialized() {
		return this.initialized >= 1;
	}

	async init(loader: ImageLoader) {
		if (this.initialized >= 1) {
			throw new Error("HandPanel already initialized");
		}

		this.initialized = 1; // loading



		const pushButton = (btn: HandButton) => {
			const parentDiv = document.createElement("div");
			const div = document.createElement("div");
			const idx = this.list.length;
			this.list.push(btn);

			parentDiv.onclick = () => {
				this.select(idx, loader);
			};

			parentDiv.appendChild(div);
			this.div.appendChild(parentDiv);

			// Load texture
			const icons = btn.getIcons();
			loader.load(icons.list).then(() => {
				setElementAsBackground(loader.get(icons.first), div);
			});
		}	

		const pushPanel = (list: HandList) => {
			const parentDiv = document.createElement("div");
			const div = document.createElement("div");
			const idx = this.list.length;
			this.list.push(list);
			parentDiv.appendChild(div);
			this.div.appendChild(parentDiv);

			// Load texture


		}

		pushButton(handlist.erase);
		pushButton(handlist.road);
		pushButton(handlist.parking);
		pushButton(handlist.turn);




		this.select(0, loader);

		this.initialized = 2; // done
		
	}
	
	cleanup() {
		if (this.initialized <= 1) {
			throw new Error("HandPanel not initialized");
		}

		this.div.innerHTML = ""; // remove children
	}


	select(idx: number, loader: ImageLoader | null) {
		this.div.children[this.selected].classList.remove('selected');
		this.div.children[idx].classList.add('selected');
		
		this.list[this.selected].diseable();
		const icon = this.list[idx].enable();

		this.selected = idx;


		if (loader && icon) {
			setElementAsBackground(
				loader.get(icon),
				this.div.children[idx].children[0] as HTMLElement
			);
		}
	}

	getButton() {
		return this.list[this.selected];
	}
}