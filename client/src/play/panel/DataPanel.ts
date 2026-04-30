import { DataReader } from "../../shared/DataReader";
import { DataWriter } from "../../shared/DataWriter";

const NO_PARENT = new Error("Missing parent element");
const htmlParent = document.getElementById("dataPanelList")!;

export class DataPanel {
	private realParent: HTMLDivElement|null = null;
	private parent: HTMLDivElement|null = null;
	callback: ((confirm: boolean)=>void)|null = null;

	open() {
		if (this.parent) {
			throw new Error("Parent already defined");
		}

		this.realParent = document.createElement("div");
		this.realParent.classList.add("dataPanel");
		this.realParent.classList.add("loadingDataPanel");

		this.parent = document.createElement("div");

		const loading = document.createElement("div");
		loading.textContent = "Loading...";

		const empty = document.createElement("div");

		const confirm = document.createElement("div");
		confirm.textContent = "Done";

		const cancel = document.createElement("div");
		cancel.textContent = "Cancel";

		confirm.onclick = () => {
			this.confirm();
		}

		cancel.onclick = () => {
			this.cancel();
		}

		this.parent.appendChild(loading);
		this.parent.appendChild(empty);
		this.parent.appendChild(confirm);
		this.parent.appendChild(cancel);
		this.realParent.appendChild(this.parent)

		htmlParent.appendChild(this.realParent);
	}


	append(
		child: HTMLElement,
		callback: ((confirm: boolean)=>void)
	) {
		if (!this.parent) {
			throw new Error("Parent already defined");
		}

		this.parent.classList.remove("loadingDataPanel");
		this.parent.children[0].classList.add("hidden");
		this.parent.children[1].appendChild(child);
		this.callback = callback;
	}

	confirm() {
		this.callback!(true);
		this.close();
	}

	cancel() {
		this.callback!(false);
		this.close();
	}

	close() {
		if (!this.realParent) {throw NO_PARENT;}

		this.realParent.remove();
		this.realParent = null;
		this.parent = null;
	}
}