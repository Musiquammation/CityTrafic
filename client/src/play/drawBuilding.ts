import { ImageLoader } from "../handler/ImageLoader";

function drawInLoop(
	img: HTMLCanvasElement | HTMLImageElement,
	ctx: OffscreenCanvasRenderingContext2D,
	size: {width: number, height: number}, 
	cellWidth: number, cellHeight: number
) {
	for (let y = 0; y < size.height; y++)
		for (let x = 0; x < size.width; x++)
			ctx.drawImage(img, x, y, 1, 1);

	return false;

}

export function drawBuilding(
	type: number,
	ctx: OffscreenCanvasRenderingContext2D,
	loader: ImageLoader,
	size: {width: number, height: number} | null
) {
	switch (type) {
		case 0: // home
		{
			ctx.drawImage(loader.get('home'), 0, 0, 2, 2);
			return false;
		}

		case 1: // oil field
		{
			if (!size) {return true;}

			drawInLoop(
				loader.get('oil'),
				ctx,
				size,
				1, 1
			);
			return false;
		}

		case 2: // plantation
		{
			if (!size) {return true;}

			drawInLoop(
				loader.get('plantation'),
				ctx,
				size,
				1, 1
			);
			return true;
		}

		case 3: // grocery
		{
			ctx.drawImage(loader.get('grocery'), 0, 0, 3, 3);
			return false;
		}

		case 4: // construction
		{
			if (!size) {return true;}

			drawInLoop(
				loader.get('constructing'),
				ctx,
				size,
				1, 1
			);
			return false;
		}

		case 5: // warehouse
		{
			ctx.drawImage(loader.get('warehouse'), 0, 0, 8, 8)
			return false;
		}
	}

	return false;
}