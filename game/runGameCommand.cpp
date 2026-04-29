#include "runGameCommand.hpp"
#include "CommandCode.hpp"

#include "Game.hpp"
#include "Character.hpp"
#include "Building.hpp"
#include "Cell.hpp"
#include "CellType.hpp"
#include "direction.hpp"
#include "jobs/OilFieldJob.hpp"

#include <stdio.h>


#define take(T) ({ T _v = *(T*)ptr; ptr = (uint8_t*)ptr + sizeof(T); _v; })
#define align(ptr,n) {ptr += n;}

struct GameCommand {
static const void* test(
	Game& game,
	const void* ptr,
	Player* player
) {
	int x = take(int32_t);
	int y = take(int32_t);
	int w = take(int32_t);
	int h = take(int32_t);

	// really bad practice
	game.calendar.hour = 6;
	game.calendar.indicator = 6*60;


	game.map.getEditCell(4, 15)->setType(CellType::PARKING, game, 0);
	game.map.getEditCell(4, 14)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(4, 13)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(4, 12)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(4, 11)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 1, 2));

	game.map.getEditCell(5, 11)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(6, 11)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 0, 3));

	game.map.getEditCell(6, 10)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(6, 9)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(6, 8)->setType(CellType::PARKING, game, 0);
	game.map.getEditCell(6, 7)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 1, 2));
	game.map.getEditCell(7, 7)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 0, 2));

	game.map.getEditCell(7, 8)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 9)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 10)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 11)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 12)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 13)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 14)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 15)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 16)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 17)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(7, 18)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 3, 2));
	game.map.getEditCell(6, 18)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(5, 18)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(4, 18)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 2, 2));
	game.map.getEditCell(4, 17)->setType(CellType::ROAD, game, 0);
	game.map.getEditCell(4, 16)->setType(CellType::ROAD, game, 0);
	


	Car* car = game.spawnCar(4, 14, Direction::UP);

	int playerId = game.getPlayerId(player);
	auto home = Building::create_home(playerId, 3, 500);
	game.map.addBuilding(10, 11, home, game);
	
	auto shop = Building::create_oilField(playerId, 1000.0f, 50000, 4);
	game.map.addBuilding(1, 5, shop, game);


	Job* job = new OilFieldJob{{1,5}, 5.0f};
	game.appendJob(job);
	job->give(10000);

	auto character = Character::spawnCharacter(game.getMap(), 10, 11);
	if (character) {
		character->give(2000);
		character->setCar(car);
		game.characterHandler.pushCharacter(character);
		character->takeJob(job, game.calendar);
	}


	return ptr;
}


static const void* erase(
	Game& game,
	const void* ptr,
	Player* player
) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::NONE, game);
	return ptr;

}

static const void* placeSingleRoad(
	Game& game,
	const void* ptr,
	Player* player
) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::ROAD, game);
	return ptr;
}

static const void* parking(
	Game& game,
	const void* ptr,
	Player* player
) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::PARKING, game);
	return ptr;

}

static const void* direction(
	Game& game,
	const void* ptr,
	Player* player
) {
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














const void* runGameCommand(
	Game& game,
	const void* ptr,
	Player* player
) {
	auto code = take(uint16_t);

	switch ((CommandCode)code) {
	case CommandCode::TEST:
		return GameCommand::test(game, ptr, player);
		
	case CommandCode::ERASE:
		return GameCommand::erase(game, ptr, player);

	case CommandCode::PLACE_SINGLE_ROAD:
		return GameCommand::placeSingleRoad(game, ptr, player);

	case CommandCode::PARKING:
		return GameCommand::parking(game, ptr, player);

	case CommandCode::DIRECTION:
		return GameCommand::direction(game, ptr, player);
	}

	return ptr;
}
