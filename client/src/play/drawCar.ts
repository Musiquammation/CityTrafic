import { Car } from "./Car";

export function drawCar(car: Car, ctx: OffscreenCanvasRenderingContext2D) {
	ctx.fillStyle = "rgb(144, 238, 144)";

	let x = .5;
	let y = .5;
	let w = 0;
	let h = 0;

	switch (car.direction) {
	case 0:
		x = car.step;
		y = .5;
		w=.9;
		h=.6;
		break;

	case 1:
		x = .5
		y = 1-car.step;
		w=.6;
		h=.9;
		break;
		
	case 2:
		x = 1-car.step;
		y = .5;
		w=.9;
		h=.6;
		break;
		
	case 3:
		x = .5
		y = car.step;
		w=.6;
		h=.9 ;       
		break;
	}

	ctx.fillRect(x, y, w, h);

}