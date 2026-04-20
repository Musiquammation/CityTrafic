#include "runGameCommand.hpp"
#include "CommandCode.hpp"

#include "Game.hpp"
#include "Cell.hpp"
#include "CellType.hpp"


#include <stdio.h>


#define take(T) ({ T _v = *(T*)args; args = (uint8_t*)args + sizeof(T); _v; })
#define move(T) {args = (uint8_t*)args + sizeof(T);}


void* run_test(Game& game, const void* args) {
	int x = take(int32_t);
	int y = take(int32_t);
	int w = take(int32_t);
	int h = take(int32_t);

	printf("Hello %d %d %d %d\n", x, y, w, h);
	return nullptr;
}


void run_placeSingleRoad(Game& game, const void* args) {
	int x = take(int32_t);
	int y = take(int32_t);


	if (!game.checkBounds(x,y,1,1))
		return;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::ROAD, game);
}














void* runGameCommand(Game& game, const void* args) {
	move(uint16_t); // for int32 padding
	auto code = take(uint16_t);

	switch ((CommandCode)code) {
	case CommandCode::TEST:
		return run_test(game, args);

	case CommandCode::PLACE_SINGLE_ROAD:
		run_placeSingleRoad(game, args);
		return nullptr;
	}

	return nullptr;
}
