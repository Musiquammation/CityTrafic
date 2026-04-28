export class ImageLoader {
	private folders: { [folderName: string]: { [imageName: string]: (HTMLImageElement | HTMLCanvasElement)[] } } = {};
	private placeholder: HTMLCanvasElement;
	private pathRoot: string;

	constructor(pathRoot: string) {
		this.pathRoot = pathRoot;

		const size = 2;
		const canvas = document.createElement('canvas');
		canvas.width = size;
		canvas.height = size;
		const ctx = canvas.getContext('2d')!;
		ctx.imageSmoothingEnabled = false;

		ctx.fillStyle = 'violet';
		ctx.fillRect(0, 0, size / 2, size / 2);
		ctx.fillRect(size / 2, size / 2, size / 2, size / 2);
		ctx.fillStyle = 'white';
		ctx.fillRect(size / 2, 0, size / 2, size / 2);
		ctx.fillRect(0, size / 2, size / 2, size / 2);
		this.placeholder = canvas;
	}

	private hexToRgb(hex: string): [number, number, number] {
		const clean = hex.replace('#', '');
		const r = parseInt(clean.substring(0, 2), 16);
		const g = parseInt(clean.substring(2, 4), 16);
		const b = parseInt(clean.substring(4, 6), 16);
		return [r, g, b];
	}

	private recolorImage(img: HTMLImageElement, checked: string, target: string): HTMLCanvasElement {
		const canvas = document.createElement('canvas');
		canvas.width = img.width;
		canvas.height = img.height;
		const ctx = canvas.getContext('2d')!;
		ctx.imageSmoothingEnabled = false;
		ctx.drawImage(img, 0, 0);

		const [cr, cg, cb] = this.hexToRgb(checked);
		const [tr, tg, tb] = this.hexToRgb(target);

		const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
		const data = imageData.data;

		for (let i = 0; i < data.length; i += 4) {
			if (data[i] === cr && data[i + 1] === cg && data[i + 2] === cb) {
				data[i] = tr;
				data[i + 1] = tg;
				data[i + 2] = tb;
			}
		}

		ctx.putImageData(imageData, 0, 0);
		return canvas;
	}

	async load(list: { [key: string]: string }): Promise<void> {
		// Initialisation du dossier si inexistant
		if (!this.folders['default']) this.folders['default'] = {};

		const entries = Object.entries(list);

		const promises = entries.map(async ([name, path]) => {
			// OPTIMISATION : Vérifie si déjà chargé
			if (this.folders['default'][name]) {
				return;
			}

			try {
				const res = await fetch(this.pathRoot + path);
				if (!res.ok) throw new Error('Failed to fetch ' + path);
				const blob = await res.blob();
				const objectUrl = URL.createObjectURL(blob);

				const img = await new Promise<HTMLImageElement>((resolve, reject) => {
					const i = new Image();
					i.onload = () => {
						URL.revokeObjectURL(objectUrl); // Nettoyage mémoire
						resolve(i);
					};
					i.onerror = e => reject(e);
					i.src = objectUrl;
				});

				this.folders['default'][name] = [img];

			} catch (err) {
				console.warn("Error with:", path, err);
			}
		});

		await Promise.all(promises);
	}

	async loadWithColors(
		checked: string,
		colors: string[],
		list: { [key: string]: string }
	): Promise<void> {
		if (!this.folders['colored']) this.folders['colored'] = {};

		const entries = Object.entries(list);

		const promises = entries.map(async ([name, path]) => {
			// Check if already loaded
			if (this.folders['colored'][name]) {
				return;
			}

			try {
				const res = await fetch(this.pathRoot + path);
				if (!res.ok) throw new Error('Failed to fetch ' + path);
				const blob = await res.blob();
				const objectUrl = URL.createObjectURL(blob);

				const img = await new Promise<HTMLImageElement>((resolve, reject) => {
					const i = new Image();
					i.onload = () => {
						URL.revokeObjectURL(objectUrl);
						resolve(i);
					};
					i.onerror = e => reject(e);
					i.src = objectUrl;
				});

				this.folders['colored'][name] = [];
				for (const color of colors) {
					const recolored = this.recolorImage(img, checked, color);
					this.folders['colored'][name].push(recolored);
				}


			} catch (err) {
				console.warn("Error with:", path, err);
			}
		});

		await Promise.all(promises);
	}

	get(name: string, color = -1): HTMLCanvasElement | HTMLImageElement {
		if (color >= 0) {
			const folder = this.folders['colored'];
			if (folder && folder[name] && folder[name][color])
				return folder[name][color];
			return this.placeholder;
		}

		const folder = this.folders['default'];
		if (folder && folder[name] && folder[name][0])
			return folder[name][0];
			
		return this.placeholder;
	}

	getFolders() {
		return this.folders;
	}
}