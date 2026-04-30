#include "Character.hpp"

#include "Job.hpp"
#include "pathfinder.hpp"
#include "Car.hpp"
#include "Game.hpp"
#include "Building.hpp"

#include "actions/action_character.hpp"

#include "DebugLogger.hpp"
static DebugLogger printSpec{"Spec", false};
static DebugLogger printStatus{"Character", true};

#include <stdio.h>

Character::Character():
	executor(actionNodes::character::init(), this, nullptr)
{
}


void Character::cleanupState() {
	switch (this->state) {
	case CharacterState::CLIENT:
		break;

	case CharacterState::WALK:
		free(this->data.walk.path);
		break;

	case CharacterState::INSIDE:
	{
		if (this->data.inside.index >= 0) {
			throw std::runtime_error{"Character has not leaved the building"};
		}
		break;
	}

	case CharacterState::OUTSIDE:
		break;

	case CharacterState::DRIVE:
		this->x = (float)this->car->x + .5f;
		this->y = (float)this->car->y + .5f;
		this->car->finishDriving(this);
		break;

	}
}

void Character::setState(CharacterState next) {
	this->cleanupState();
	this->state = next;
}

ActionCode Character::walk(Game& game) {
	if (this->state != CharacterState::WALK) {
		throw std::runtime_error{"Character is not walking"};
	}

	char dir = this->data.walk.path[this->data.walk.position];
	if (dir == 8)
		return ActionCode::SUCCESS;

	static constexpr float SQRT2 = 1.41421356f;
	static constexpr float INV_SQRT2 = 0.70710678f;
	static const Vector<float> DELTAS[] = {
		{ 1.0f,  0.0f},              // right
		{ INV_SQRT2, -INV_SQRT2},    // up-right
		{ 0.0f, -1.0f},              // up
		{-INV_SQRT2, -INV_SQRT2},    // up-left
		{-1.0f,  0.0f},              // left
		{-INV_SQRT2,  INV_SQRT2},    // down-left
		{ 0.0f,  1.0f},              // down
		{ INV_SQRT2,  INV_SQRT2}     // down-right
	};

	static const Vector<int> DIRECTIONS[] = {
		{ 1,  0},   // right
		{ 1, -1},   // up-right
		{ 0, -1},   // up
		{-1, -1},   // up-left
		{-1,  0},   // left
		{-1,  1},   // down-left
		{ 0,  1},   // down
		{ 1,  1}    // down-right
	};


	auto move = [this, &dir, &game]() {
		this->data.walk.anchor.x += DIRECTIONS[dir].x;
		this->data.walk.anchor.y += DIRECTIONS[dir].y;
		this->data.walk.position++;

		dir = this->data.walk.path[this->data.walk.position];

		return dir == 8;
	};

	// Move step
	this->data.walk.step += Character::SPEED;
	if (dir%2 == 0) {
		if (this->data.walk.step > 1) {
			this->data.walk.step -= 1;
			if (move()) {return ActionCode::SUCCESS;}
		}
	} else if (this->data.walk.step > SQRT2) {
		this->data.walk.step -= SQRT2;
		if (move()) {return ActionCode::SUCCESS;}
	}

	this->x = (float)this->data.walk.anchor.x + .5f + DELTAS[dir].x * this->data.walk.step;
	this->y = (float)this->data.walk.anchor.y + .5f + DELTAS[dir].y * this->data.walk.step;


	return ActionCode::PENDING;
}

CharacterState Character::getState() const {
	return this->state;
}

Character* Character::createClientCharacter(float x, float y) {
	auto c = new Character;
	c->x = x;
	c->y = y;
	c->state = CharacterState::CLIENT;
	return c;
}

Character* Character::spawnCharacter(const Map& map, int x, int y) {
	auto info = map.getBuilding(x, y);
	if (!info.building)
		return nullptr;

	if (info.building->isFull())
		return nullptr;


	auto c = new Character;
	int index = info.building->enter(c);
	if (index < 0) {
		delete c;
		return nullptr;
	}

	c->x = (float)x + .5f;
	c->y = (float)y + .5f;
	c->home = {info.x, info.y};
	c->state = CharacterState::INSIDE;
	c->money = 0;
	c->data.inside.index = index;
	c->pointId = 0;

	return c;
}


#include <chrono>
#include <iostream>
bool Character::makeWalk(Game& game, int destX, int destY) {
	auto start = std::chrono::high_resolution_clock::now();

	int x = (int)this->x;
	int y = (int)this->y;
	
	char* path;
	// Character already placed
	if (x == destX && y == destY) {
		path = (char*)malloc(1);
		path[0] = 8;

	} else {
		path = makePedestranPath(
			game.getMap(),
			x,
			y,
			destX,
			destY
		);
	}

	handlePath:


	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);


	printSpec("Execution time %ld µs\n", duration.count());



	if (!path) {
		return false;
	}

	printSpec("Pedestran path from (%d %d) to (%d %d): ",
		x, y, destX, destY);

	for (char* i = path; *i != 8; i++)
		printSpec("%d ", *i);
	printSpec("\n");

	this->setState(CharacterState::WALK);
	this->data.walk.path = path;
	this->data.walk.position = 0;
	this->data.walk.step = 0;
	this->data.walk.anchor = {x, y};
	return true;
}

bool Character::makeDrive(Map& map, int destX, int destY) {
	if (!this->car)
		return false;

	if (!this->car->drive(this, destX, destY, map))
		return false;
		
	this->setState(CharacterState::DRIVE);
	this->data.drive.state = ActionCode::PENDING;
	return true;
}

bool Character::makeInside(Game& game) {
	auto info = game.getBuilding((int)this->x, (int)this->y);
	if (!info.building)
		return false;

	int place = info.building->enter(this);
	if (place < 0)
		return false;

	this->setState(CharacterState::INSIDE);
	this->data.inside.index = place;
	return true;
}

void Character::makeOutside(Game& game) {
	// Leave place
	if (this->state == CharacterState::INSIDE) {
		auto info = game.getMap().getBuilding(
			(int)this->x,
			(int)this->y
		);

		if (!info.building) {
			throw std::runtime_error{"Character is not placed on a building"};
		}

		info.building->leave(this->data.inside.index);

		// Place character outside
		Vector<int> point;
		{
			int largeLength = info.building->getBufferLargeLength();
			Vector<int> leaveList[largeLength];
			int length = info.building->fillLeaveList(leaveList);
			point = leaveList[this->takeRandomPointId(length)];
		}

		this->x = (float)(info.x + point.x) + .5f;
		this->y = (float)(info.y + point.y) + .5f;

		this->data.inside.index = -1; // Mark outside
	} else {
		printWarn("Trying to leave while not inside\n");
	}


	
	this->setState(CharacterState::OUTSIDE);

}


void Character::notifyDrive() {
	this->data.drive.state = ActionCode::SUCCESS;
}

BuildingInfo Character::getHomeBuilding(const Map& map) const {
	/// TODO: rework getHomeBuilding
	return map.getBuilding(this->home.x, this->home.y);
}

BuildingInfo Character::getWorkBuilding(Game& game) const {
	/// TODO: rework getWorkBuilding
	if (this->job) {
		auto site = this->job->getEmployeeSite(this, game.getCalendar());
		return game.getMap().getBuilding(site.x, site.y);
	}

	return {INT32_MIN, INT32_MIN, nullptr};
}

bool Character::orientBuilding(Game& game, BuildingInfo info) {
	int largeLength = info.building->getBufferLargeLength();
	
	Vector<int> point;
	{
		int largeLength = info.building->getBufferLargeLength();
		Vector<int> leaveList[largeLength];
		int length = info.building->fillLeaveList(leaveList);
		point = leaveList[this->takeRandomPointId(length)];
	}

	return this->makeWalk(game, info.x + point.x, info.y + point.y);
}

bool Character::locateBuilding(Map& map, BuildingInfo info) {
	int largeLength = info.building->getBufferLargeLength();
	
	Vector<int> point;
	{
		int largeLength = info.building->getBufferLargeLength();
		Vector<int> leaveList[largeLength];
		int length = info.building->fillLeaveList(leaveList);
		point = leaveList[this->takeRandomPointId(length)];
	}

	return this->makeDrive(map, info.x + point.x, info.y + point.y);
}




void Character::frame(Game& game) {
	if (this->state == CharacterState::CLIENT)
		return;
		
	if (this->executor.run(game, this)) {
		this->executor.restart();
	}
}

int Character::takeRandomPointId(int modulo) {
	int exit = this->pointId % modulo;
	this->pointId++;
	return exit;
}


Car* Character::getCar() const {
	return this->car;
}

bool Character::setCar(Car* car) {
	switch (this->state) {
	// Forbid car edit
	case CharacterState::CLIENT:
	case CharacterState::DRIVE:
		return false;

	default:
	{
		this->car = car;	
		return true;
	}
	}
}


bool Character::takeJob(Job* job, const Calendar& calendar) {
	if (this->job) {
		this->leaveJob();
	}

	if (job->hire(this, calendar)) {
		this->job = job;
		return true;
	}

	return false;
}

void Character::leaveJob() {
	this->job->fire(this);
	this->job = nullptr;
}

Job* Character::getJob() {
	return this->job;
}

void Character::give(int money) {
	this->money += money;
}

int Character::pay(int money) {
	if (this->money >= money) {
		this->money -= money;
		return money;
	}

	int given = this->money;
	this->money = 0;
	return given;

}



Character::~Character() {
	this->cleanupState();	
}