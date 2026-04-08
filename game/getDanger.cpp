#include "getDanger.hpp"

#include "utils/mfor.hpp"
#include "utils/mergeSortedVectors.hpp"

#include "Car.hpp"
#include "PathHandler.hpp"
#include "Game.hpp"
#include "Cell.hpp"
#include "debugFile.hpp"
#include "PriorityNode.hpp"


#include <limits>
#include <vector>
#include <math.h>


static constexpr float INFINITY_F = std::numeric_limits<float>::infinity();

float computeAcceleration(
	float vx0, float vy0,
	float a_y,
	float vx_max, float vy_max,
	float X, float Y
) {
	// Helper: compute y(t) saturation parameters
	float t_star;
	float Vysat;

	if (a_y > 0.0f) {
		Vysat = vy_max;
		t_star = (vy_max - vy0) / a_y;
	} else if (a_y < 0.0f) {
		Vysat = 0.0f;
		t_star = -vy0 / a_y;
	} else { // a_y == 0
		Vysat = vy0;
		t_star = INFINITY_F;
	}

	// Compute T such that y(T) = Y
	float T;

	if (a_y > 0.0f) {
		float discriminant = vy0 * vy0 + 2.0f * a_y * Y;
		if (discriminant < 0.0f) return INFINITY_F;

		float T_before = (-vy0 + std::sqrt(discriminant)) / a_y;

		if (T_before <= t_star) {
			T = T_before;
		} else {
			float y_star = vy0 * t_star + 0.5f * a_y * t_star * t_star;
			T = t_star + (Y - y_star) / Vysat;
		}

	} else if (a_y == 0.0f) {
		if (vy0 <= 0.0f) return INFINITY_F;
		T = Y / vy0;

	} else { // a_y < 0
		float discriminant = vy0 * vy0 - 2.0f * a_y * Y;
		if (discriminant < 0.0f) return INFINITY_F;

		float T_before = (-vy0 - std::sqrt(discriminant)) / a_y;

		if (T_before <= t_star) {
			T = T_before;
		} else {
			float y_max = vy0 * t_star + 0.5f * a_y * t_star * t_star;
			if (Y > y_max) return INFINITY_F;
			T = t_star;
		}
	}

	// Avoid division by zero
	if (T <= 0.0f) return INFINITY_F;

	// Compute candidate a_x without x saturation
	float a_x_no_sat = 2.0f * (X - vx0 * T) / (T * T);

	if (a_x_no_sat > 0.0f) {
		if (vx0 + a_x_no_sat * T > vx_max) {
			return 0.5f * vx_max * vx_max - vx0 * vx0 / X;
		}
	} else if (a_x_no_sat < 0.0f) {
		if (vx0 + a_x_no_sat * T < 0.0f) {
			return -0.5f * vx0 * vx0 / X;
		}
	}

	return a_x_no_sat;
}

struct Spy {
	int x;
	int y;
	Direction dir;

	void move() {
		this->x += Direction_getVector(this->dir).x;
		this->y += Direction_getVector(this->dir).y;
	}
};






int fillGraph(
	int frontDist, int sideDist,
	int range,
	bool addEmptyChild,
	Game* game,
	std::vector<PriorityNode>& priorities,
	Spy spy
) {
	if (range <= 0)
		return -1;
		
	
	auto cell = game->getCell(spy.x, spy.y);

	
	// Adapt to road
	switch (cell->getType()) {
	case CellType::NONE:
		return -1;

	// case CellType::YIELD:
		// if (addEmptyChild)
			// goto checkRoad;
		// continue to ROAD behavior	
		
	case CellType::ROAD:
	{
		int children[3];
		int childrenNodeIdx = -1;

		Car* car;
		if (cell->hasCar()) {
			car = game->getCar(spy.x, spy.y);
		} else {
			car = nullptr;
		}



		{
			// Check in front
			Spy fspy = spy;
			fspy.move();
			children[0] = fillGraph(frontDist, sideDist+1,
				range-1, false, game, priorities, fspy);
	
	
			// check to the right
			Spy rspy = spy;
			rspy.dir = Direction_getRight(spy.dir);
			rspy.move();
	
			children[1] = fillGraph(frontDist, sideDist+1,
				range-1, false, game, priorities, rspy);
	
			
			// Check to the left
			Spy lspy = spy;
			lspy.dir = Direction_getRight(spy.dir);
			lspy.move();
	
			children[2] = fillGraph(frontDist, sideDist+1,
				range-1, false, game, priorities, lspy);
		}

		

		int validLength = 0;
		for (int i = 0; i < 3; i++)
			if (children[i] != -1)
				validLength++;
		

		if (validLength == 1 && !car) {
			for (int i = 0; true; i++)
				if (children[i] >= 0)
					return children[i];
		}

		if (validLength == 0 && !car)
			return -1;

		int fullValid = validLength + (addEmptyChild?0:1);
		int* childrenArr = (int*)malloc((fullValid+1) * sizeof(PriorityNode));
			
		int j = 0;
		for (int i = 0; i < 3; i++) {
			if (children[i] >= 0) {
				childrenArr[j] = children[i];
				j++;
			}
		}
		childrenArr[j] = -1;
			

		priorities.push_back({
			frontDist,
			sideDist,
			childrenArr,
			car
		});

		return (int)priorities.size() - 1;
		break;
	}


	default:
		break;
	
	}


	return -1;
}


float getNodeAcc(
	const Car* car,
	float maxAcceleration,
	std::vector<PriorityNode>& priorities,
	PriorityNode* node
) {
	float carEntryDist = (float)node->frontDist - car->step +
		(1.0f + (1.0f-Car::HEIGHT)/2 - Car::WIDTH/2);

	float carExitDist = carEntryDist + Car::HEIGHT;

	float sideEntryDist = (float)node->sideDist +
		(Car::HEIGHT + Car::WIDTH)/2 - car->step;

	float sideExitDist = sideEntryDist + 1 + (Car::WIDTH - Car::HEIGHT)/2;

	float fastAcc = computeAcceleration(
		car->getSpeed(), node->car->getSpeed(),
		node->car->getAcceleration(),
		car->speedLimit, node->car->speedLimit,
		carExitDist, sideEntryDist
	);

	if (fastAcc == INFINITY_F)
		fastAcc = -INFINITY_F;

	
	// Needs to wait the car with the priority
	float slowAcc = computeAcceleration(
		car->getSpeed(), node->car->getSpeed(),
		node->car->getAcceleration(),
		car->speedLimit, node->car->speedLimit,
		carEntryDist, sideExitDist
	);

	// if (slowAcc == INFINITY_F) // is this case even possible?
		// slowAcc = maxAcceleration;


	// Pass BEFORE the car (so try to use fastAcc)
	if (fastAcc <= maxAcceleration) {
		float min = maxAcceleration;
		for(int* cptr = node->children; true; cptr++) {
			int c = *cptr;
			if (c < 0)
				break;
			float acc = getNodeAcc(car, maxAcceleration,
				priorities, &priorities[c]);

			if (acc < min)
				min = acc;
		}

		if (min < fastAcc) {
			// Needs to wait a car, so acceleration is reduced
			// Let's make shure our slowAcc is verified
			if (min > slowAcc)
				min = slowAcc;
		}

		return min;
	}
	
	// Wait for the car and its children
	
	float min = slowAcc;
	for(int* cptr = node->children; true; cptr++) {
		int c = *cptr;
		if (c < 0)
			break;
		float acc = getNodeAcc(car, maxAcceleration,
			priorities, &priorities[c]);

		if (acc < min)
			min = acc;
	}

	return slowAcc;
}


getDanger_t getDanger(
	const Car* car,
	Game* game,
	std::vector<PriorityNode>& priorities
) {
	enum {
		FRONT_RANGE = 32,
		SIDE_RANGE = 16
	};


	auto pathHandler = PathHandler<false>{car->pathHandler};

	float maxAcceleration = Car::MAX_ACCELERATION;
	const float speedLimit = car->speedLimit;
	const float carSpeed = car->getSpeed();
	const float carSpeed2 = carSpeed * carSpeed;
	Vector<int> targetPoint{-1, -1};

	const auto appendStopDist = [
		carSpeed, carSpeed2, &targetPoint,
		speedLimit, &maxAcceleration
	](float dist, float deceleration, Vector<int> targetPt) {

		#define stopDist ((.5f/deceleration) * carSpeed2)
		// Slow down
		if (dist <= 0) {
			maxAcceleration = -carSpeed;
			targetPoint = targetPt;

		} else if (dist < stopDist) {
			float acc = -.5f * carSpeed2 / dist;
			if (acc < maxAcceleration) {
				targetPoint = targetPt;
				maxAcceleration = acc;
			}
			

		} else if (maxAcceleration > 0) {
			float acc = (speedLimit - carSpeed) * (1.0f/Car::SPEED_FACTOR);
			if (acc < maxAcceleration) {
				targetPoint = targetPt;
				maxAcceleration = acc;
			}
		}

		// Check if next speed will exceed
		if (carSpeed + maxAcceleration >= dist) {
			targetPoint = targetPt;
			maxAcceleration = dist - carSpeed;
		}

		
		#undef stopDist
	};



	Spy spy{car->x, car->y, car->direction};
	Vector<int> pathPoint = pathHandler.seek();
	
	
	// Get terrain speed limit and check priorities
	for (int dist = 0; dist <= FRONT_RANGE; dist++) {
		auto cell = game->getCell(spy.x, spy.y);
		CellType cellType = cell->getType();
		const Car* other = cell->hasCar() ?
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
			
			int previousNodeIdx = (int)priorities.size() - 1;
			int nodeIdx = fillGraph(dist, 0, SIDE_RANGE - dist,
				true, game, priorities, checker);

			
			if (nodeIdx >= 0) {
				if (previousNodeIdx >= 0) {
					PriorityNode& previous = priorities[previousNodeIdx];
					for (int* ptr = previous.children; true; ptr++) {
						int i = *ptr;
						if (i >= 0)
							continue;

						*ptr = nodeIdx;
						ptr++;
						*ptr = -1;

						break;
					}
				}

				previousNodeIdx = nodeIdx;
			}


			// PriorityNode& node = priorities[nodeIdx];
		}



		finishRightPriority:


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

	// Get priority acceleration
	if (priorities.size() >= 1) {
		maxAcceleration = getNodeAcc(car, maxAcceleration, priorities, &priorities[0]);
	}



	for (auto i: priorities)
		free(i.children);

	priorities.clear();

	return {
		maxAcceleration,
		targetPoint
	};
}
