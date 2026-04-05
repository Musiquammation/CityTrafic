#include "Car.hpp"

Car::Car(int x, int y, Direction direction) 
	: x(x), y(y), direction(direction)
{}

void Car::update() {

}

void Car::move() {

}

Vector<float> Car::calcPosition() {
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