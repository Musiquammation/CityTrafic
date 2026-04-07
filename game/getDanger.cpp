#include "getDanger.hpp"

#include "Car.hpp"
#include "PathHandler.hpp"
#include "Game.hpp"
#include "Cell.hpp"
#include <limits>


typedef struct {
	int x;
	int y;
	Direction dir;


	void move() {
		this->x += Direction_getVector(this->dir).x;
		this->y += Direction_getVector(this->dir).y;
	}

} Spy;

getDanger_t getDanger(const Car* car, Game* game) {
	enum {
		FRONT_RANGE = 32
	};

	static constexpr float INFINITY = 
		std::numeric_limits<float>::infinity();

	auto pathHandler = PathHandler<false>{car->pathHandler};
	

	float bestAcceleration = 0;
	const float speedLimit = car->speedLimit;
	const float carSpeed = car->speed;
	const float carSpeed2 = carSpeed * carSpeed;
	const float stopDist = (.5f/Car::SOFT_DECELERATION) * carSpeed2;

	const auto appendStopDist = [stopDist, carSpeed, carSpeed2,
		speedLimit, &bestAcceleration](float dist)
	{

		printf("L: %f\n", stopDist);
		// Slow down
		if (dist < stopDist) {
			float acc = -.5f * carSpeed2 / dist;
			if (acc < bestAcceleration)
				bestAcceleration = acc;

			return;
		}

		// Accelerate
		if (bestAcceleration < 0)
			return;

		float acc = (speedLimit - carSpeed) * (1.0f/Car::SPEED_FACTOR);
		if (acc > bestAcceleration)
			bestAcceleration = acc;
		
	};


	Spy spy{car->x, car->y, car->direction};
	Vector<int> pathPoint = pathHandler.seek();

	for (int dist = 1; dist <= FRONT_RANGE; dist++) {
		// Check if we need to turn
		if (spy.x == pathPoint.x && spy.y == pathPoint.y) {
			Direction aim = pathHandler.seekDirection();
			pathPoint = pathHandler.seek();

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
		// Car* other = game->getCar(spy.x, spy.y);
		Car* other = nullptr;
		
		// Handle cell
		switch (cellType) {
		case CellType::NONE:
		{
			appendStopDist((float)dist - car->step - Car::WIDTH/2);
			goto finishUpdate;
		}

		case CellType::ROAD:
		{
			if (other) {
				// appendStopDist((float)dist + other->step - car->step - Car::WIDTH);
			}
			break;
		}		
		}


		// Check right priority


		
	}


	finishUpdate:

	


	return {
		bestAcceleration
	};
}
