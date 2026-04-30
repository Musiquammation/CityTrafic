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

	export const f32: Unit<number> = {
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
}
