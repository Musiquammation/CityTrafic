export function drawCell(data: number, ctx: OffscreenCanvasRenderingContext2D) {
	switch (data & 0xf) {
	case 0: // nothing
		break;

	case 1: // road
		ctx.fillStyle = "red";
		ctx.fillRect(0, 0, 1, 1);
		break;


	case 2: // building
		ctx.fillStyle = "#008000";
		ctx.fillRect(0, 0, 1, 1);
		break

	case 3: // link
		ctx.fillStyle = "#00ff00";
		ctx.fillRect(0, 0, 1, 1);
		break;

	}
}