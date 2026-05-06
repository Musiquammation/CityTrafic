import { DataReader } from "../../shared/DataReader"
import { DataWriter } from "../../shared/DataWriter";

export interface Unit<T, Html extends HTMLElement = HTMLElement> {
	read: (reader: DataReader) => T;
	write: (writer: DataWriter, data: T) => void;
	create: (name: string, data: T) => Html;
	finish: (element: Html) => T;
}

export namespace units {
	export const i32: Unit<number> = {
		read: reader => {
			const value = reader.readInt32();
			return value;
		},

		write: (writer, data) => {
			
		},

		create: (name, data) => {
			const div = document.createElement("div");

			const span = document.createElement("span");
			span.textContent = name;

			const input = document.createElement("span");
			input.textContent = data.toString();

			div.appendChild(span);
			div.appendChild(input);

			return div;
		},

		finish: element => {
			return 0;
		}
	}

	export const f32: Unit<number> = {
		read: reader => {
			const value = reader.readFloat32();
			return value;
		},

		write: (writer, data) => {
			
		},

		create: (name, data) => {
			const div = document.createElement("div");

			const span = document.createElement("span");
			span.textContent = name;

			const input = document.createElement("span");
			input.textContent = data.toString();

			div.appendChild(span);
			div.appendChild(input);

			return div;
		},

		finish: element => {
			return 0;
		}
	}

	export const mut_i32: Unit<number> = {
		read: reader => {
			const value = reader.readInt32();
			return value;
		},

		write: (writer, data) => {
			writer.writeInt32(data);
		},

		create: (name, data) => {
			const div = document.createElement("div");

			const span = document.createElement("span");
			span.textContent = name;

			const input = document.createElement("input");
			input.type = "number";
			input.value = data.toString();

			div.appendChild(span);
			div.appendChild(input);

			return div;
		},

		finish: element => {
			const input = element.children[1] as HTMLInputElement;

			if (!input || input.tagName !== "INPUT") {
				throw new Error("Invalid structure");
			}

			return Math.floor(Number(input.value));
		}
	}

	export const mut_f32: Unit<number> = {
		read: reader => {
			return reader.readFloat32();
		},

		write: (writer, data) => {
			writer.writeFloat32(data);
		},

		create: (name, data) => {
			const div = document.createElement("div");

			const span = document.createElement("span");
			span.textContent = name;

			const input = document.createElement("input");
			input.type = "number";
			input.value = data.toString();

			div.appendChild(span);
			div.appendChild(input);

			return div;
		},

		finish: element => {
			const input = element.children[1] as HTMLInputElement;

			if (!input || input.tagName !== "INPUT") {
				throw new Error("Invalid structure");
			}

			return Number(input.value);
		}
	}

	export const btn: Unit<boolean> = {
		read: reader => {
			return (reader.readUint32() != 0);
		},

		write: (writer, data) => {
			writer.writeUint32(data ? 1:0);
		},

		create: (name, data) => {
			const div = document.createElement("div");

			// const span = document.createElement("span");
			// span.textContent = name;

			const btn = document.createElement("button");
			btn.textContent = name;

			// div.appendChild(span);
			div.appendChild(btn);

			btn.onclick = ()=>{btn.classList.add("btn-pressed");}

			return div;
		},

		finish: element => {
			return element.children[0].classList.contains("btn-pressed");
		}

	}
}
