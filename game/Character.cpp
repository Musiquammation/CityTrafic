#include "Character.hpp"

#include "pathfinder.hpp"
#include "Car.hpp"
#include "Game.hpp"
#include "Building.hpp"


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
	return c;
}


bool Character::makeWalk(Game& game, int destX, int destY) {
	this->state = CharacterState::WALK;

	auto path = new PathHandler<true>;
	if (!makePedestranPath(game.getMap(), *path)) {
		delete path;
		return false;
	}


	this->state = CharacterState::WALK;
	this->data.walk.path = path;
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



Character::~Character() {
	this->cleanupState();	
}