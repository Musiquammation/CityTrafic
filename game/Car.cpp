#include "Car.hpp"


#include "Character.hpp"
#include "getDanger.hpp"
#include "Game.hpp"
#include "Cell.hpp"
#include "debugFile.hpp"


Car::Car(int x, int y, Direction direction) 
	: x(x), y(y), direction(direction)
{}





void Car::update(Game* game, std::vector<PriorityNode>& prioritiesBuffer) {
	auto danger = getDanger(this, game, prioritiesBuffer);


	this->realTargetPoint = danger.targetPoint;

	if (danger.acceleration < 0) {
		this->realSpeed += danger.acceleration;
		if (this->realSpeed < 0) {
			this->realSpeed = 0;
		}

	} else if (danger.acceleration > 0) {
		this->realSpeed += danger.acceleration;
		if (this->realSpeed > this->speedLimit) {
			this->realSpeed = this->speedLimit;
		}
	}

}

void Car::move() {
	// Make members public
	this->publicAcceleration = this->realSpeed - this->publicSpeed; // newSpeed - oldSpeed
	this->publicSpeed = this->realSpeed;
	this->publicTargetPoint = this->realTargetPoint;
	
	// Move
	this->step += this->realSpeed;
	if (this->step >= 1) {
		this->step -= 1;

		switch (this->state) {
		case CarState::FRONT:
			break;

		case CarState::TURN_RIGHT:
			this->direction = Direction_getRight(this->direction);
			break;

		case CarState::TURN_LEFT:
			this->direction = Direction_getLeft(this->direction);
			break;
		}


		this->x += Direction_getVector(this->direction).x;
		this->y += Direction_getVector(this->direction).y;
	}
}

Vector<float> Car::calcPosition() const {
	switch (this->state) {
	case CarState::FRONT:
	{
		switch (this->direction) {
		case Direction::RIGHT:
			return Vector<float>{
				(float)this->x + this->step,
				(float)this->y + .5f
			};

		case Direction::LEFT:
			return Vector<float>{
				(float)this->x + 1 - this->step,
				(float)this->y + .5f
			};

		case Direction::UP:
			return Vector<float>{
				(float)this->x + .5f,
				(float)this->y + 1 - this->step
			};

		case Direction::DOWN:
			return Vector<float>{
				(float)this->x + .5f,
				(float)this->y + this->step
			};
		}

		break;
	}

	case CarState::TURN_RIGHT:
	{

		break;
	}

	case CarState::TURN_LEFT:
	{

		break;
	}


	}

	return Vector<float>{0,0};
}



float Car::getSpeed() const {
	return this->publicSpeed;
}

float Car::getAcceleration() const {
	return this->publicAcceleration;
}

Vector<int> Car::getTargetPoint() const {
	return this->publicTargetPoint;
}

void Car::setSpeed(float speed) {
	this->publicSpeed = speed;
	this->realSpeed = speed;
}


bool Car::drive(Character* driver, int destX, int destY) {
	if (this->driver)
		return false;

	this->driver = driver;

	/// TODO: find path according to driver aim

	return true;
}

void Car::finishDriving() {
	this->driver->notifyDrive();
	this->driver = nullptr;
}