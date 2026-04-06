#include "Car.hpp"
#include "Game.hpp"

Car::Car(int x, int y, Direction direction) 
	: x(x), y(y), direction(direction)
{}



typedef struct {
	int x;
	int y;
	Direction dir;


	void move() {
		this->x += Direction_getVector(this->dir).x;
		this->y += Direction_getVector(this->dir).x;
	}

} Spy;

void Car::update(Game* game) {
	enum {
		VIEW_RANGE = 16
	};


	Spy spy{this->x, this->y, this->direction};

	for (int dist = 1; dist <= VIEW_RANGE; dist++) {
		spy.move();

		
	}
}

void Car::move() {
	this->step += this->speed;

	if (this->step >= 1) {
		this->step -= 1;

		this->x += Direction_getVector(this->direction).x;
		this->y += Direction_getVector(this->direction).y;
	}
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