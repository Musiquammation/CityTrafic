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
	

	float bestAcceleration = Car::MAX_ACCELERATION;
	const float speedLimit = car->speedLimit;
	const float carSpeed = car->speed;
	const float carSpeed2 = carSpeed * carSpeed;

	const auto appendStopDist = [ carSpeed, carSpeed2,
		speedLimit, &bestAcceleration](float dist, float deceleration)
	{

		#define stopDist ((.5f/deceleration) * carSpeed2)
		// printf("d: %2.3f ; ", dist);
		// Slow down
		if (dist <= 0) {
			bestAcceleration = -carSpeed;

		} else if (dist < stopDist) {
			float acc = -.5f * carSpeed2 / dist;
			if (acc < bestAcceleration)
				bestAcceleration = acc;
			
			// printf("aS: %2.3f ; ", bestAcceleration);

		} else if (bestAcceleration > 0) {
			float acc = (speedLimit - carSpeed) * (1.0f/Car::SPEED_FACTOR);
			if (acc < bestAcceleration)
				bestAcceleration = acc;

		}

		// Check if next speed will exceed
		if (carSpeed + bestAcceleration >= dist) {
			bestAcceleration = dist - carSpeed;
		}

		
		// printf("aM: %2.3f ; ", bestAcceleration);
		#undef stopDist
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
		Car* other = cell->hasCar() ?
			game->getCar(spy.x, spy.y) :
			nullptr;
		
		// Handle cell
		switch (cellType) {
		case CellType::NONE:
		{
			appendStopDist(
				(float)dist - car->step - Car::WIDTH/2,
				Car::SOFT_DECELERATION
			);
			goto finishUpdate;
		}

		case CellType::ROAD:
		{
			if (other) {
				appendStopDist(
					(float)dist + other->step - car->step - Car::WIDTH,
					Car::FRONT_DECELERATION
				);
			}
			break;
		}		
		}


		// Check right priority


		
	}


	finishUpdate:

	

	// if (bestAcceleration < -Car::MAX_DECELERATION)
		// bestAcceleration = -Car::MAX_DECELERATION;

	return {
		bestAcceleration
	};
}
