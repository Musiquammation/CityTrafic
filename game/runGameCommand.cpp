#include "runGameCommand.hpp"
#include "CommandCode.hpp"

#include "Game.hpp"
#include "Character.hpp"
#include "Building.hpp"
#include "Cell.hpp"
#include "CellType.hpp"
#include "direction.hpp"

#include <stdio.h>


#define take(T) ({ T _v = *(T*)ptr; ptr = (uint8_t*)ptr + sizeof(T); _v; })
#define align(ptr,n) {ptr += n;}

struct GameCommand {
static const void* test(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);
	int w = take(int32_t);
	int h = take(int32_t);


	game.map.getEditCell(2, 14)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(2, 13)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(2, 12)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(2, 11)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 1, 2));

	game.map.getEditCell(3, 11)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(4, 11)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 0, 3));

	game.map.getEditCell(4, 10)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(4, 9)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(4, 8)->setType(CellType::PARKING, game, 0);

	Car* car = game.spawnCar(2, 14, Direction::UP);

	auto home = Building::create_home(3);
	game.map.addBuilding(10, 12, home, game);
	
	auto shop = Building::create_home(3);
	game.map.addBuilding(1, 5, home, game);

	auto character = Character::spawnCharacter(game.getMap(), 10, 12);
	if (character) {
		character->setCar(car);
		game.characterHandler.pushCharacter(character);
	}

	return ptr;
}


static const void* erase(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::NONE, game);
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

static const void* parking(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::PARKING, game);
	return ptr;

}

static const void* direction(Game& game, const void* ptr) {
	int x = take(int32_t);
	int y = take(int32_t);
	cell_t args = take(uint16_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::DIRECTION, game, args);
	return ptr;

}




};














const void* runGameCommand(Game& game, const void* ptr) {
	auto code = take(uint16_t);

	switch ((CommandCode)code) {
	case CommandCode::TEST:
		return GameCommand::test(game, ptr);
		
	case CommandCode::ERASE:
		return GameCommand::erase(game, ptr);

	case CommandCode::PLACE_SINGLE_ROAD:
		return GameCommand::placeSingleRoad(game, ptr);

	case CommandCode::PARKING:
		return GameCommand::parking(game, ptr);

	case CommandCode::DIRECTION:
		return GameCommand::direction(game, ptr);
	}

	return ptr;
}
