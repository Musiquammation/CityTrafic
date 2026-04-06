#include "Car.hpp"
#include "Game.hpp"
#include "Cell.hpp"

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
	Vector<int> pathPoint = this->pathHandler.seek();


	for (int dist = 1; dist <= VIEW_RANGE; dist++) {
		// Check if we need to turn
		if (spy.x == pathPoint.x && spy.y == pathPoint.y) {
			Direction aim = this->pathHandler.seekDirection();
			pathPoint = this->pathHandler.seek();

			int turn = Direction_getTurn(spy.dir, aim);
			if (turn == 1) {
				spy.dir = Direction_getRight(spy.dir);
			} else if (turn == -1) {
				spy.dir = Direction_getLeft(spy.dir);
			} else {
				throw std::domain_error{"turn is 0"};
			}
		}

		// Move
		spy.move();

		Cell* cell = game->getCell(spy.x, spy.y);
		CellType cellType = cell->getType();
		
		// Handle cell
		switch (cellType) {
		case CellType::NONE:
			goto finishUpdate;


		}


		// Check right priority
		

		
	}


	finishUpdate:
	return;
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