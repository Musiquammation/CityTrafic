#include "getDanger.hpp"

#include "Car.hpp"
#include "PathHandler.hpp"
#include "Game.hpp"
#include "Cell.hpp"

#include <limits>
#include <vector>


typedef struct {
	int x;
	int y;
	Direction dir;

	void move() {
		this->x += Direction_getVector(this->dir).x;
		this->y += Direction_getVector(this->dir).y;
	}
} Spy;

typedef struct {
	float slowAcc; // acceleration when we pass after  the car
	float fastAcc; // acceleration when we pass before the car
} PriorityAcceleration;

getDanger_t getDanger(const Car* car, Game* game) {
	enum {
		FRONT_RANGE = 32,
		SIDE_RANGE = 16
	};

	static constexpr float INFINITY = 
		std::numeric_limits<float>::infinity();

	auto pathHandler = PathHandler<false>{car->pathHandler};
	std::vector<PriorityAcceleration> accelerations{};

	float bestAcceleration = Car::MAX_ACCELERATION;
	const float speedLimit = car->speedLimit;
	const float carSpeed = car->getSpeed();
	const float carSpeed2 = carSpeed * carSpeed;
	Vector<int> targetPoint{-1, -1};

	const auto appendStopDist = [
		carSpeed, carSpeed2, &targetPoint,
		speedLimit, &bestAcceleration
	](float dist, float deceleration, Vector<int> targetPt) {

		#define stopDist ((.5f/deceleration) * carSpeed2)
		// printf("d: %2.3f ; ", dist);
		// Slow down
		if (dist <= 0) {
			bestAcceleration = -carSpeed;
			targetPoint = targetPt;

		} else if (dist < stopDist) {
			float acc = -.5f * carSpeed2 / dist;
			if (acc < bestAcceleration) {
				targetPoint = targetPt;
				bestAcceleration = acc;
			}
			
			// printf("aS: %2.3f ; ", bestAcceleration);

		} else if (bestAcceleration > 0) {
			float acc = (speedLimit - carSpeed) * (1.0f/Car::SPEED_FACTOR);
			if (acc < bestAcceleration) {
				targetPoint = targetPt;
				bestAcceleration = acc;
			}

		}

		// Check if next speed will exceed
		if (carSpeed + bestAcceleration >= dist) {
			targetPoint = targetPt;
			bestAcceleration = dist - carSpeed;
		}

		
		// printf("aM: %2.3f ; ", bestAcceleration);
		#undef stopDist
	};

	/**
	 * @return `true` if should stop checking line
	 */
	const auto applyPriority = [
		car, &accelerations
	](int intFrontDist, int intSideDist, Car* other) {
		float carEntryDist = (float)intFrontDist + car->step +
			(1.0f-Car::HEIGHT-Car::WIDTH)/2;

		float carExitDist = carEntryDist + Car::HEIGHT;

		float sideEntryDist = (float)intSideDist +
			(Car::HEIGHT + Car::WIDTH)/2 - car->step;

		float sideExitDist = sideEntryDist + 1 + (Car::WIDTH - Car::HEIGHT)/2;



		return true;
	};

	Spy spy{car->x, car->y, car->direction};
	Vector<int> pathPoint = pathHandler.seek();

	for (int dist = 0; dist <= FRONT_RANGE; dist++) {
		auto cell = game->getCell(spy.x, spy.y);
		CellType cellType = cell->getType();
		Car* other = cell->hasCar() ?
			game->getCar(spy.x, spy.y) :
			nullptr;
		

		bool checkRightPriority;
		bool checkLeftPriority;
		// Handle cell
		switch (cellType) {
		case CellType::NONE:
		{
			appendStopDist(
				(float)dist - car->step - Car::WIDTH/2,
				Car::SOFT_DECELERATION,
				{spy.x, spy.y}
			);
			goto finishUpdate;
		}

		case CellType::ROAD:
		{
			if (other && other != car) {
				appendStopDist(
					(float)dist + other->step - car->step - Car::WIDTH,
					Car::FRONT_DECELERATION,
					{spy.x, spy.y}
				);
			}
			checkRightPriority = true;
			checkLeftPriority = false;
			break;
		}

		default:
			checkRightPriority = false;
			checkLeftPriority = false;
			break;
		}

		if (dist >= SIDE_RANGE)
			goto finishLeftPriority;

		// Check right priority
		if (checkRightPriority) {
			// Place front right
			Spy checker{spy};
			const Direction otherDir = Direction_getLeft(spy.dir);

			checker.move();
			checker.dir = Direction_getRight(spy.dir);
			checker.move();
			

			// Get first cell
			auto checkCell = game->getCell(checker.x, checker.y);
			switch (checkCell->getType()) {
			case CellType::NONE:
			// case CellType::YIELD:
			{
				goto finishRightPriority;
			}

			case CellType::ROAD:
			{
				if (!checkCell->hasCar())
					break;
			
				Car* other = game->getCar(checker.x, checker.y);
				if (!other || other == car || other->direction != otherDir)
					break;

				if (applyPriority(dist, 0, other))
					goto finishRightPriority; // finish

				break;
			}
			}


		}

		finishRightPriority:


		if (checkLeftPriority) {

		}

		finishLeftPriority:



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
	}


	finishUpdate:

	

	// if (bestAcceleration < -Car::MAX_DECELERATION)
		// bestAcceleration = -Car::MAX_DECELERATION;

	return {
		bestAcceleration,
		targetPoint
	};
}
