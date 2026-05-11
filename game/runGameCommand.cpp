#include "runGameCommand.hpp"
#include "CommandCode.hpp"

#include "Game.hpp"
#include "Character.hpp"
#include "Building.hpp"
#include "Cell.hpp"
#include "CellType.hpp"
#include "direction.hpp"
#include "jobs/OilFieldJob.hpp"
#include "jobs/CashierJob.hpp"
#include "jobs/ConstructionJob.hpp"


#include "jobs/AgricultorJob.hpp"


#define take(T) ({ T _v = *(T*)ptr; ptr = (uint8_t*)ptr + sizeof(T); _v; })
#define align(ptr,n) {ptr += n;}

#define def(name) static const void* name(\
	Game& game,\
	const void* ptr,\
	Player* player\
)

struct GameCommand {
def(test) {
	int x = take(int32_t);
	int y = take(int32_t);
	int w = take(int32_t);
	int h = take(int32_t);

	printf("RUN_TEST_COMMAND\n");


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
	game.map.addBuilding(10, 15, home, game);

	for (int i = 0; i < 3; i++) {
		auto character = Character::spawnCharacter(game.getMap(), 10, 11);
		character->give(2000);
		character->setCar(car);
		game.characterHandler.pushCharacter(character);
	}

	auto oilJob = new OilFieldJob{1.9f, 2.0f};
	oilJob->give(10000);
	oilJob->employeesCounters.raffiners.goal = 1;
	auto oilField = Building::create_oilField(oilJob, playerId, 1000.0f, 50000, 4);
	game.map.addBuilding(1, 5, oilField, game);
	oilField->oilField.refined += 100;

	auto cashierJob = new CashierJob{};
	auto grocery = Building::create_grocery(cashierJob, playerId);
	game.map.addBuilding(15, 1, grocery, game);
	grocery->grocery.stock += 1000.0f;

	auto agricultureJob = new AgricultorJob{0.5f, 0.4f};
	agricultureJob->employeesCounters.agricultors.goal = 1;
	game.map.addBuilding(8, 20, Building::create_plantation(agricultureJob, playerId, 10), game);


	return ptr;
}


def(erase) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::NONE, game);
	return ptr;

}

def(placeSingleRoad) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::ROAD, game);
	return ptr;
}

def(parking) {
	int x = take(int32_t);
	int y = take(int32_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::PARKING, game);
	return ptr;

}

def(direction) {
	int x = take(int32_t);
	int y = take(int32_t);
	cell_t args = take(uint16_t);

	if (!game.checkBounds(x,y,1,1))
		return ptr;

	Cell* cell = game.getEditCell(x,y);
	cell->setType(CellType::DIRECTION, game, args);
	return ptr;

}

def(placeHome) {
	int x = take(int32_t);
	int y = take(int32_t);
	int constructionMoney = take(int32_t);
	int capacity = take(int32_t);
	int rent = take(int32_t);
	int playerId = game.getPlayerId(player);

	auto building = Building::create_home(playerId, capacity, rent);
	auto constructionJob = new ConstructionJob{
		(float)constructionMoney / (float)building->getConstructionCost()
	};
	auto construction = Building::create_construction(
		constructionJob,
		building,
		playerId
	);

	if (!game.map.addBuilding(x, y, construction, game)) {
		delete construction;
		delete building;
		delete constructionJob;
	}


	return ptr;
}

def(placeGrossery) {
	return ptr;
}


};














const void* runGameCommand(
	Game& game,
	const void* ptr,
	Player* player
) {
	// Check 16bit aligment
	auto offset16 = (uintptr_t)ptr;
	offset16 = (offset16 + 1) & ~uintptr_t(1);
	ptr = (const void*)offset16;


	auto code = take(uint16_t);

	// Check 32bit aligment
	auto offset32 = (uintptr_t)ptr;
	offset32 = (offset32 + 3) & ~uintptr_t(3);
	ptr = (const void*)offset32;

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

	case CommandCode::PLACE_HOME:
		return GameCommand::placeHome(game, ptr, player);

	case CommandCode::PLACE_GROSSERY:
		return GameCommand::placeGrossery(game, ptr, player);

	}

	return ptr;
}
