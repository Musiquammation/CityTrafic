#include "Game.hpp"

#include "CellInstruction.hpp"
#include "JobOffer.hpp"
#include "Car.hpp"
#include "Character.hpp"
#include "Cell.hpp"
#include "Job.hpp"
#include "Map.hpp"
#include "Building.hpp"

#include <stdio.h>
#include <math.h>

static constexpr int LIGHT_COULDOWN = 150;

void Game::test() {
	
}

int Game::searchPlayer(hash_t key) {
	int size = (int)this->players.size();
	for (int i = 0; i < size; i++) {
		if (this->players[i].key == key)
			return (int)i;
	}

	this->players.push_back(Player{hash_generate()});
	return size;
}

void Game::applyLightLogic() {
	this->lightCooldownLeft--;
	if (this->lightCooldownLeft <= 0) {
		this->lightCooldownLeft += LIGHT_COULDOWN;
		this->lightCooldownValue++;
		if (this->lightCooldownValue >= 8)
			this->lightCooldownValue -= 8;
	}
}


void Game::start() {
	
}

void Game::frame() {
	this->test();


	// Car logic
	this->carHandler.updateCars(this);
	this->carHandler.moveCars();


	// Character logic
	for (Character* character : this->characterHandler) {
		character->frame(*this);
	}

	// Update grid
	this->map.resetCarMarks();
	for (auto& [pos, car] : this->carHandler) {
		auto cell = this->map.getShadowEditCell(car->x, car->y);
		cell->setCarOn();
	}


	// Light logic
	this->applyLightLogic();


	// Calendar
	this->calendar.move();

	// Frame count
	this->frameCount++;
}

Cell* Game::getEditCell(int x, int y) {
	return this->map.getEditCell(x, y);
}

const Cell* Game::getCell(int x, int y) {
	return this->map.getCell(x, y);
}

Car* Game::spawnCar(int x, int y, Direction direction) {
	auto cell = this->map.getEditCell(x, y);
	if (!cell || cell->hasCar()) {
		return nullptr;
	}

	Car* car = this->carHandler.spawnCar(x, y, direction);
	cell->setCarOn();
	return car;
}

Car* Game::getCar(int x, int y) {
	return this->carHandler.getCar(x, y);
}

BuildingInfo Game::getBuilding(int x, int y) const {
	if (x == INT32_MIN)
		return {INT32_MIN, INT32_MIN, nullptr};

	return this->map.getBuilding(x, y);
}

void Game::destroyBuilding(BuildingInfo info) {
	auto bounds = info.building->getSize();

	bounds.x += info.x;
	bounds.y += info.y;

	info.building->destroy(*this);
	delete info.building;


	// Clear area
	cell_t arg = (cell_t)CellInstruction::BUILDING << 4;
	for (int y = info.y; y < bounds.y; y++) { 
		for (int x = info.x; x < bounds.x; x++) {
			this->getEditCell(x,y)->setType(
				CellType::NONE,
				*this,
				arg
			);
		}
	}
}





int Game::getFrame() const {
	return this->frameCount;
}


bool Game::checkBounds(int x, int y, int width, int height) const {
	return this->map.checkBounds(x,y,width,height);
}

Player* Game::getPlayer(int id) {
	if (id < 0) {
		return nullptr;
	}

	return &this->players[id];
}

int Game::getPlayerId(Player* player) {
	if (player == nullptr)
		return -1;

	return int(player - this->players.data());
}


Vector<int> Game::searchJob(
	Character* c,
	JobOffer& bestOffer
) {
	static constexpr int MAX_RADIUS = 64.0f;

	auto home = c->getHome();
	if (home.x == INT32_MIN) {
		home = c->getPos();
	}
	

	float bestScore = 0;
	Vector<int> bestLoc{INT32_MIN, INT32_MIN};

	
	for (
		auto it = this->map.buildings_begin();
		it != this->map.buildings_end();
		it++
	) {
		auto building = it->second;
		auto job = building->getJob();
		if (!job)
			continue;

		JobOffer offer;
		if (!job->searchJobOffer(c, offer))
			continue;

		auto loc = it->first;
		auto pos = job->getEmployeeSite(c, loc, building, this->calendar);
		int dx = pos.x - home.x;
		int dy = pos.y - home.y;
		float score = (MAX_RADIUS - sqrtf(float(dx*dx + dy*dy)));
		if (score < 0)
			continue;

		score *= (float)offer.salaryEstimation;

		if (score > bestScore) {
			bestScore = score;
			bestLoc = loc;
			bestOffer = offer;
		}
	}	
	return bestLoc;
}


Vector<int> Game::searchHome(
	int maxRent,
	int cx,
	int cy
) {
	static constexpr int RADIUS = 250;

	for (auto it = this->map.buildings_begin();
		it != this->map.buildings_end();
		it++
	) {
		auto building = it->second;
		if (
			building->type != BuildingType::HOME ||
			building->home.left <= 0 ||
			building->home.rent > maxRent
		) {
			continue;
		}



		Vector<int> pos = it->first;
		int dx = pos.x - cx;
		int dy = pos.y - cy;
		if (dx*dx + dy*dy > RADIUS * RADIUS)
			continue;

		printf("=> %d %d\n", pos.x, pos.y);
		return pos;
	}

	return {INT32_MIN, INT32_MIN};
}

Game::Game(int width, int height):
	map(width, height),
	lightCooldownLeft(LIGHT_COULDOWN)
{
	printf("build\n");
}


Game::~Game() {

}
