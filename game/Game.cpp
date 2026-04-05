#include "Game.hpp"


void Game::frame() {

	this->carHandler.updateCars();

	this->carHandler.moveCars();

	this->map.resetCarMarks();
	
}