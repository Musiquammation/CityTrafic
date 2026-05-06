import { ImageLoader } from "../handler/ImageLoader";

export function loadAssets(loader: ImageLoader) {
    loader.load({
		turn: "assets/turn/turn.png",
		all: "assets/turn/all.png",
		select: "assets/turn/select.png",
		share: "assets/turn/share.png",
		home: "assets/home.png",
		constructing: "assets/constructing.png",
		grocery: "assets/grocery.png",
		parking: "assets/parking.png",
		oil: "assets/oil.png",
		plantation: "assets/plantation.png",
    });
}