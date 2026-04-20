import { Character } from "./Character";

export function drawCharacter(c: Character, ctx: OffscreenCanvasRenderingContext2D) {
	ctx.fillStyle = "rgb(144, 127, 255)";
	ctx.fillRect(0, 0, 1, 1);
}
