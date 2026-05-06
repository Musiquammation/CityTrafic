import { ImageLoader } from "../handler/ImageLoader";
import { direction } from "./direction";
import { drawBuilding } from "./drawBuilding";

const DIRECTION_NAMES = ['all', 'front', 'turn', 'turn', 'select', 'select', 'share', 'all'];
const DIRECTION_FLIPS = [false, false, false, true, false, true, false];

export function drawCell(
	data: number,
	ctx: OffscreenCanvasRenderingContext2D,
	loader: ImageLoader
) {
	function drawImage(name: string, angle: number, flip = {x: false, y: false, color: -1}) {
		ctx.save();
		ctx.translate(0.5, 0.5);
		ctx.rotate(-angle);
		ctx.scale(flip.x ? -1 : 1, flip.y ? -1 : 1);
		ctx.imageSmoothingEnabled = false;
		ctx.drawImage(loader.get(name, flip.color), -0.5, -0.5, 1, 1);
		ctx.restore();
	}

	switch (data & 0xf) {
	case 0: // nothing
		break;

	case 1: // road
		ctx.fillStyle = "#777";
		ctx.fillRect(0, 0, 1, 1);
		break;


	case 2: // building
		return drawBuilding(data >> 4, ctx, loader, null);

	case 3: // link
		if (data & (1<<14)) {
			/// TODO: draw link cell
		}
		break;

	case 4: // parking
		ctx.drawImage(loader.get('parking'), 0, 0, 1, 1);
		break;

	case 5: // direction
	{
		const used: number[][] = [];
		for (const d of [0, 1, 2, 3]) {
			const s = direction.getSide(data, d);
			if (s !== 0) {
				used.push([s, d]);
			}
		}
	

		if (used.length === 1) {
			const side = used[0][0];
			const dir = Math.PI/2 * used[0][1];
			drawImage(DIRECTION_NAMES[side], dir, {
				x: false,
				y: DIRECTION_FLIPS[side],
				color: -1
			});
			break
		}

		if (used.length === 2) {
			/// TODO: handle this case
			ctx.fillStyle = "#333";
			ctx.fillRect(0, 0, 1, 1);
			break;
		}

		// No used or corrupted state
		ctx.fillStyle = "#333";
		ctx.fillRect(0, 0, 1, 1);
		break;

	}
	}


	return false;
}