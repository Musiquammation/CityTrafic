import { ImageLoader } from "../handler/ImageLoader";
import { sendCommand } from "../net/sendCommand";
import { COMMAND_CODES } from "../shared/CommandCode";
import { handlist } from "./hands/handlist";
import { HandButton, HandObject } from "./hands/handtypes";










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


		const promises: Promise<void>[] = [];

		const pushButton = async (btn: HandButton) => {
			
			const parentDiv = document.createElement("div");
			const div = document.createElement("div");
			const idx = this.list.length;
			this.list.push(btn);

			parentDiv.onclick = () => {
				console.log(idx);
				this.select(idx);
			};

			parentDiv.appendChild(div);
			this.div.appendChild(parentDiv);
		}	

		const pushPanel = async () => {

		}

		promises.push(pushButton(handlist.erase));
		promises.push(pushButton(handlist.road));



		await Promise.all(promises);

		this.select(0);

		this.initialized = 2; // done
		
	}
	
	cleanup() {
		if (this.initialized <= 1) {
			throw new Error("HandPanel not initialized");
		}

		this.div.innerHTML = ""; // remove children
	}


	select(idx: number) {
		this.div.children[this.selected].classList.remove('selected');
		this.div.children[idx].classList.add('selected');
		
		this.list[this.selected].diseable();
		this.list[idx].enable();

		this.selected = idx;
	}

	getButton() {
		return this.list[this.selected];
	}
}