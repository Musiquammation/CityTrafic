import { GameHandler } from "./handler/GameHandler";

let _gameHandler: GameHandler | null = null;

export function setGameHandler(gameHandler: GameHandler) {
    _gameHandler = gameHandler;
}

export function getGameHandler() {
    if (_gameHandler === null)
        throw new Error("Missing gameHandler");
    
    return _gameHandler;
}
