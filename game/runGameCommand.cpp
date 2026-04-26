#include "runGameCommand.hpp"
#include "CommandCode.hpp"

#include "Game.hpp"
#include "Building.hpp"
#include "Cell.hpp"
#include "CellType.hpp"


#include <stdio.h>


#define take(T) ({ T _v = *(T*)ptr; ptr = (uint8_t*)ptr + sizeof(T); _v; })
#define align(ptr,n) {ptr += n;}

struct GameCommand {
static const void* test(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);
	int w = take(int32_t);
	int h = take(int32_t);


	auto building = Building::create_home(3);
	auto success = game.map.addBuilding(10, 12, building, game);

	printf("Hello %d\n", success);
	return ptr;
}


static const void* placeSingleRoad(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::ROAD, game);
	return ptr;
}



};














const void* runGameCommand(Game& game, const void* ptr) {
	auto code = take(uint16_t);

	switch ((CommandCode)code) {
	case CommandCode::TEST:
		return GameCommand::test(game, ptr);

	case CommandCode::PLACE_SINGLE_ROAD:
		return GameCommand::placeSingleRoad(game, ptr);
	}

	return ptr;
}
