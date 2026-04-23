#include "runGameCommand.hpp"
#include "CommandCode.hpp"

#include "Game.hpp"
#include "Cell.hpp"
#include "CellType.hpp"


#include <stdio.h>


#define take(T) ({ T _v = *(T*)ptr; ptr = (uint8_t*)ptr + sizeof(T); _v; })
#define push(T, val) {*(T*)res = (T)val; res += sizeof(T);}
#define align(ptr,n) {ptr += n;}
#define retRes() {*(uint32_t*)response =  (uint32_t)(res - response - 4); return response;}


void* run_test(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);
	int w = take(int32_t);
	int h = take(int32_t);

	printf("Hello %d %d %d %d\n", x, y, w, h);
	return nullptr;
}


void run_placeSingleRoad(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);


	if (!game.checkBounds(x,y,1,1))
		return;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::ROAD, game);
}














void* runGameCommand(Game& game, const void* ptr) {
	align(ptr,3);
	auto code = take(uint16_t);

	switch ((CommandCode)code) {
	case CommandCode::TEST:
		return run_test(game, ptr);

	case CommandCode::PLACE_SINGLE_ROAD:
		run_placeSingleRoad(game, ptr);
		return nullptr;
	}

	return nullptr;
}
