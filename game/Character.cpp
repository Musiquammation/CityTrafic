#include "Character.hpp"

#include "pathfinder.hpp"
#include "Car.hpp"
#include "Game.hpp"
#include "Building.hpp"

#include "actions/action_character.hpp"


Character::Character():
	executor(actionNodes::character::init(), this, nullptr)
{

}


void Character::cleanupState() {
	switch (this->state) {
	case CharacterState::CLIENT:
		break;

	case CharacterState::WALK:
		delete this->data.walk.path;
		break;

	case CharacterState::INSIDE:
		break;

	case CharacterState::OUTSIDE:
		break;

	case CharacterState::DRIVE:
		/// TODO: notify car?
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
	c->homePosition = -1;
	return c;
}

Character* Character::spawnCharacter(const Map& map, int x, int y) {
	auto info = map.getBuilding(x, y);
	if (!info.building)
		return nullptr;

	if (info.building->type != BuildingType::HOME)
		return nullptr;

	if (info.building->home.isFull())
		return nullptr;


	auto c = new Character;
	int homePosition = info.building->home.add(c);
	if (homePosition < 0) {
		delete c;
		return nullptr;
	}

	c->x = (float)x + .5f;
	c->y = (float)y + .5f;
	c->state = CharacterState::INSIDE;
	c->homePosition = homePosition;
	c->pointId = 0;

	return c;
}


bool Character::makeWalk(Game& game, int destX, int destY) {
	auto path = makePedestranPath(
		game.getMap(),
		(int)this->x,
		(int)this->y,
		destX,
		destY
	);

	if (!path) {
		return false;
	}


	this->setState(CharacterState::WALK);
	this->data.walk.path = path;
	this->data.walk.position = 0;
	this->data.walk.step = 0;
	this->data.walk.anchor = {(int)this->x, (int)this->y};
	return true;
}

bool Character::makeDrive(Game& game, int destX, int destY) {
	if (!this->car)
		return false;

	if (!this->car->drive(this, destX, destY, game.getMap()))
		return false;
		
	this->state = CharacterState::DRIVE;
	return true;
}

bool Character::makeInside(Game& game) {
	auto info = game.getBuilding((int)this->x, (int)this->y);
	if (!info.building)
		return false;

	int place = info.building->home.add(this);
	if (place < 0)
		return false;

	this->state = CharacterState::INSIDE;
	return true;
}




void Character::notifyDrive() {
	this->data.drive.state = ActionCode::SUCCESS;
}

BuildingInfo Character::getHomeBuilding(const Map& map) const {
	/// TODO: rework getWorkBuilding
	return map.getBuilding(10, 12);
}

BuildingInfo Character::getWorkBuilding(const Map& map) const {
	/// TODO: rework getWorkBuilding
	return map.getBuilding(1, 5);
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

bool Character::locateBuilding(Game& game, BuildingInfo info) {
	int largeLength = info.building->getBufferLargeLength();
	
	Vector<int> point;
	{
		int largeLength = info.building->getBufferLargeLength();
		Vector<int> leaveList[largeLength];
		int length = info.building->fillLeaveList(leaveList);
		point = leaveList[this->takeRandomPointId(length)];
	}

	return this->makeDrive(game, info.x + point.x, info.y + point.y);
}




void Character::frame(Game& game) {
	if (this->state == CharacterState::CLIENT)
		return;
		
	this->executor.run(game, this);
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



Character::~Character() {
	this->cleanupState();	
}