import { direction as direction_helper } from "../direction";

// Direction entries for the dropdown menus
const DIRECTIONS = [
	{ value: 0, label: "RIGHT" },
	{ value: 1, label: "UP" },
	{ value: 2, label: "LEFT" },
	{ value: 3, label: "DOWN" },
] as const;

// Sub-direction checkboxes (relative to the chosen direction)
const SUB_OPTIONS = [
	{ key: "front" as const, label: "Front" },
	{ key: "right" as const, label: "Right" },
	{ key: "left"  as const, label: "Left"  },
];

// Convert three booleans to the side enum value
// 0=nothing 1=front 2=right 3=left 4=front-right 5=front-left 6=left-right 7=all
function bitsToValue(front: boolean, right: boolean, left: boolean): number {
	if (front && right && left) return 7;
	if (right && left)		  return 6;
	if (front && left)		  return 5;
	if (front && right)		 return 4;
	if (left)				   return 3;
	if (right)				  return 2;
	if (front)				  return 1;
	return 0;
}

// Decompose a side enum value into three booleans
function valueToBits(v: number): { front: boolean; right: boolean; left: boolean } {
	return {
		front: v === 1 || v === 4 || v === 5 || v === 7,
		right: v === 2 || v === 4 || v === 6 || v === 7,
		left:  v === 3 || v === 5 || v === 6 || v === 7,
	};
}

// Find the two active directions in data, falling back to defaults if needed
function findDirections(data: number): [number, number] {
	const used: number[] = [];
	for (const d of [0, 1, 2, 3]) {
		if (direction_helper.getSide(data, d) !== 0) used.push(d);
	}

	const dir0 = used[0] ?? 0;
	// Pick a second direction different from the first
	const dir1 = used[1] ?? [0, 1, 2, 3].find(d => d !== dir0)!;
	return [dir0, dir1];
}

export class TurnSelector {
	parent: HTMLElement;
	private callback: ((data: number | null) => void) | null = null;
	private currentData = 0;

	constructor(parent: HTMLElement) {
		this.parent = parent;
		this.parent.querySelector("#ts-ok")!
			.addEventListener("click", () => this.confirm());
		this.parent.querySelector("#ts-cancel")!
			.addEventListener("click", () => this.cancel());
	}

	// Open the selector with the given data, call callback with updated data or null on cancel
	take(data: number, callback: (data: number | null) => void): void {
		this.currentData = data;
		this.callback = callback;
		this.parent.classList.remove("hidden");
		this.buildUI(data);
	}

	// Build both rows from the current data
	private buildUI(data: number): void {
		const [dir0, dir1] = findDirections(data);
		this.buildRow(0, dir0, dir1, data);
		this.buildRow(1, dir1, dir0, data);
	}

	// Build a single direction row (select + checkboxes)
	private buildRow(
		rowIndex: 0 | 1,
		direction: number,
		otherDirection: number,
		data: number
	): void {
		const row = this.parent.querySelector(`#ts-row-${rowIndex}`) as HTMLElement;
		row.innerHTML = "";

		// Row label
		const lbl = document.createElement("label");
		lbl.textContent = rowIndex === 0 ? "1st direction" : "2nd direction";
		row.appendChild(lbl);

		// Direction select — exclude the direction used by the other row
		const select = document.createElement("select");
		for (const d of DIRECTIONS) {
			if (d.value === otherDirection) continue;
			const opt = document.createElement("option");
			opt.value = String(d.value);
			opt.textContent = d.label;
			opt.selected = d.value === direction;
			select.appendChild(opt);
		}
		row.appendChild(select);

		// Checkboxes for front / right / left
		const checkDiv = document.createElement("div");
		checkDiv.className = "ts-checkboxes";

		const currentValue = direction_helper.getSide(data, direction);
		const bits = valueToBits(currentValue);

		for (const sub of SUB_OPTIONS) {
			const lbl2 = document.createElement("label");
			const cb = document.createElement("input");
			cb.type = "checkbox";
			cb.dataset.key = sub.key;
			cb.checked = bits[sub.key];
			lbl2.appendChild(cb);
			lbl2.append(" " + sub.label);
			checkDiv.appendChild(lbl2);
		}

		row.appendChild(checkDiv);

		// Rebuild both rows when direction changes to avoid duplicates
		select.addEventListener("change", () => this.onDirectionChange(rowIndex));
	}

	// Handle direction change: resolve conflicts and rebuild both rows
	private onDirectionChange(changedRow: 0 | 1): void {
		const otherRow = (changedRow === 0 ? 1 : 0) as 0 | 1;

		const changedSelect = this.parent.querySelector(
			`#ts-row-${changedRow} select`
		) as HTMLSelectElement;
		const otherSelect = this.parent.querySelector(
			`#ts-row-${otherRow} select`
		) as HTMLSelectElement;

		const newDir	  = parseInt(changedSelect.value);
		const currentOther = parseInt(otherSelect.value);

		if (newDir === currentOther) {
			// Conflict: assign the other row a free direction
			const fallback = [0, 1, 2, 3].find(d => d !== newDir)!;
			this.buildRow(changedRow, newDir,	 fallback, this.currentData);
			this.buildRow(otherRow,   fallback,   newDir,   this.currentData);
		} else {
			this.buildRow(changedRow, newDir,		currentOther, this.currentData);
			this.buildRow(otherRow,   currentOther,  newDir,	   this.currentData);
		}
	}

	// Read the current state of both rows
	private readRows(): { dir: number; value: number }[] {
		return ([0, 1] as const).map(rowIndex => {
			const select = this.parent.querySelector(
				`#ts-row-${rowIndex} select`
			) as HTMLSelectElement;
			const checkDiv = this.parent.querySelector(
				`#ts-row-${rowIndex} .ts-checkboxes`
			) as HTMLElement;

			const dir = parseInt(select.value);

			let front = false, right = false, left = false;
			checkDiv.querySelectorAll<HTMLInputElement>("input[type=checkbox]").forEach(cb => {
				if (cb.dataset.key === "front") front = cb.checked;
				if (cb.dataset.key === "right") right = cb.checked;
				if (cb.dataset.key === "left")  left  = cb.checked;
			});

			return { dir, value: bitsToValue(front, right, left) };
		});
	}

	// Apply the selected values to data and call the callback
	private confirm(): void {
		const rows = this.readRows();
		let data = this.currentData;

		// Clear directions not selected in either row
		for (const d of [0, 1, 2, 3]) {
			if (d !== rows[0].dir && d !== rows[1].dir) {
				const result = direction_helper.setSide(data, d, 0);
				if (result !== 0) data = result;
			}
		}

		// Write the two selected directions
		for (const { dir, value } of rows) {
			const result = direction_helper.setSide(data, dir, value);
			if (result !== 0) data = result;
		}

		this.close(data);
	}

	private cancel(): void {
		this.close(null);
	}

	private close(result: number | null): void {
		this.parent.classList.add("hidden");
		if (this.callback) {
			this.callback(result);
			this.callback = null;
		}
	}
}

export const turnSelector = new TurnSelector(
	document.getElementById("turnSelector")!
);