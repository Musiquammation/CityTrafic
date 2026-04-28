import { ImageLoader } from "../handler/ImageLoader";

export function loadAssets(loader: ImageLoader) {
    loader.load({
		turn: "assets/turn/turn.png",
		all: "assets/turn/all.png",
		select: "assets/turn/select.png",
		share: "assets/turn/share.png",
    });
}