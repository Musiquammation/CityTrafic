#include "runGameCommand.hpp"
#include "CommandCode.hpp"

#include "Game.hpp"
#include "Cell.hpp"
#include "CellType.hpp"


#include <stdio.h>


#define take(T) ({ T _v = *(T*)ptr; ptr = (uint8_t*)ptr + sizeof(T); _v; })
#define align(ptr,n) {ptr += n;}


const void* run_test(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);
	int w = take(int32_t);
	int h = take(int32_t);

	printf("Hello %d %d %d %d\n", x, y, w, h);
	return ptr;
}


const void* run_placeSingleRoad(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::ROAD, game);
	return ptr;
}














const void* runGameCommand(Game& game, const void* ptr) {
	auto code = take(uint16_t);

	switch ((CommandCode)code) {
	case CommandCode::TEST:
		return run_test(game, ptr);

	case CommandCode::PLACE_SINGLE_ROAD:
		return run_placeSingleRoad(game, ptr);
	}

	return ptr;
}
