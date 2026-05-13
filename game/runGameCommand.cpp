#include "runGameCommand.hpp"

#include "CommandCode.hpp"
#include "Direction.hpp"

#include "Game.hpp"
#include "Character.hpp"
#include "Building.hpp"
#include "Cell.hpp"
#include "CellType.hpp"
#include "direction.hpp"
#include "jobs/OilFieldJob.hpp"
#include "jobs/CashierJob.hpp"
#include "jobs/ConstructionJob.hpp"

#include <stdio.h>

#include "jobs/TruckJob.hpp"


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




	for (int i = 1; i < 18; i++) {
		game.map.getEditCell(16, i)->setType(CellType::ROAD, game, 0);
		game.map.getEditCell(6, i)->setType(CellType::ROAD, game, 0);
	}

	for (int i = 6; i < 16; i++) {
		game.map.getEditCell(i, 1)->setType(CellType::ROAD, game, 0);
		game.map.getEditCell(i, 18)->setType(CellType::ROAD, game, 0);
	}

	game.map.getEditCell(16, 6)->setType(CellType::PARKING, game, 0);
	game.map.getEditCell(16, 13)->setType(CellType::PARKING, game, 0);
	game.map.getEditCell(13, 17)->setType(CellType::PARKING, game, 0);
	game.map.getEditCell(6, 6)->setType(CellType::PARKING, game, 0);
	game.map.getEditCell(6, 13)->setType(CellType::PARKING, game, 0);
	game.map.getEditCell(6, 17)->setType(CellType::PARKING, game, 0);


	game.map.getEditCell(6, 18)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 2, 2));
	game.map.getEditCell(16, 1)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 0, 2));
	game.map.getEditCell(6, 1)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 1, 2));
	game.map.getEditCell(16, 18)->setType(CellType::DIRECTION, game,
		direction::setSide(0, 3, 2));


	Car* cars[] = {
		game.spawnCar(16, 13, Direction::DOWN),
		// game.spawnCar(6, 14, Direction::UP),
		// game.spawnCar(16, 17, Direction::UP),
	};


	int playerId = game.getPlayerId(player);
	
	auto home = Building::create_home(playerId, 3, 500);
	game.map.addBuilding(10, 4, home, game);

	for (int i = 0; i < 1; i++) {
		auto character = Character::spawnCharacter(game.getMap(), 10, 11);
		character->give(2000);
		character->setCar(cars[0]);
		game.characterHandler.pushCharacter(character);

		printf("Character #%d: %p\n", i, character);

	}

	auto oilJob = new OilFieldJob{1.9f, 2.0f};
	oilJob->give(10000);
	oilJob->employeesCounters.raffiners.goal = 1;
	auto oilField = Building::create_oilField(oilJob, playerId, 1000.0f, 50000, 4);
	game.map.addBuilding(1, 5, oilField, game);
	oilField->oilField.refined += 100;

	// auto cashierJob = new CashierJob{};
	// auto grocery = Building::create_grocery(cashierJob, playerId);
	// game.map.addBuilding(1, 9, grocery, game);
	// grocery->grocery.stock += 1000.0f;

	auto agricultureJob = new AgricultorJob{0.5f, 0.4f};
	agricultureJob->employeesCounters.agricultors.goal = 1;
	game.map.addBuilding(8, 20, Building::create_plantation(agricultureJob, playerId, 10), game);

	// auto truckJob = new TruckJob{};
	// truckJob->salaryPerHour = 15.0f;
	// truckJob->employeesCounters.truckers.goal = 1;
	// game.map.addBuilding(19, 10, Building::create_warehouse(truckJob, playerId), game);
	//



	for (auto& player: game.players)
		player.money += 5000;

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
	int x = take(int32_t);
	int y = take(int32_t);
	int constructionMoney = take(int32_t);
	int playerId = game.getPlayerId(player);

	auto job = new CashierJob{};

	auto building = Building::create_grocery(job, playerId);
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

def(placeTruck) {
	int x = take(int32_t);
	int y = take(int32_t);
	int constructionMoney = take(int32_t);
	int playerId = game.getPlayerId(player);

	auto job = new TruckJob{};

	auto building = Building::create_warehouse(job, playerId);
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

	case CommandCode::PLACE_TRUCK:
		return GameCommand::placeTruck(game, ptr, player);
	}

	return ptr;
}
