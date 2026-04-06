#include "Car.hpp"

#include "getDanger.hpp"
#include "Game.hpp"
#include "Cell.hpp"


Car::Car(int x, int y, Direction direction) 
	: x(x), y(y), direction(direction)
{}





void Car::update(Game* game) {
	auto danger = getDanger(this, game);
}

void Car::move() {
	this->step += this->speed;

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
				this->x + this->step,
				(float)this->y
			};

		case Direction::LEFT:
			return Vector<float>{
				this->x - this->step,
				(float)this->y
			};

		case Direction::UP:
			return Vector<float>{
				(float)this->x,
				this->y - this->step
			};

		case Direction::DOWN:
			return Vector<float>{
				(float)this->x,
				this->y + this->step
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