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

	case CharacterState::DRIVE:
		/// TODO: notify car?
		break;

	}
}

void Character::setState(CharacterState next) {
	this->cleanupState();
	this->state = next;
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
	c->x = (float)x + .5f;
	c->y = (float)y + .5f;
	c->state = CharacterState::INSIDE;
	int homePosition = info.building->home.add(c);
	if (homePosition < 0) {
		delete c;
		return nullptr;
	}

	c->homePosition = homePosition;

	

	return c;
}


bool Character::makeWalk(Game& game, int destX, int destY) {
	this->state = CharacterState::WALK;

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


	this->state = CharacterState::WALK;
	this->data.walk.path = path;
	this->data.walk.position = 0;
	return true;
}

bool Character::makeDrive(Game& game, int destX, int destY) {
	if (!this->car)
		return false;

	if (!this->car->drive(this, destX, destY))
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

}

void Character::frame(Game& game) {
	this->executor.run(game, this);
}



Character::~Character() {
	this->cleanupState();	
}