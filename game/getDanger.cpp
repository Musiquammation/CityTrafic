#include "getDanger.hpp"

#include "utils/mfor.hpp"
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
	int frontDist;
	int sideDist;
	const Car* other;
	Vector<int> targetPoint;
	std::vector<int> children{};


	PriorityNode build() {
		PriorityNode node;
		node.frontDist = frontDist;
		node.sideDist = sideDist;
		node.other = other;
		node.childrenLength = static_cast<int>(children.size());
		node.targetPoint = targetPoint;

		if (!children.empty()) {
			node.children = new int[children.size()];
			for (size_t i = 0; i < children.size(); ++i) {
				node.children[i] = children[i];
			}
		} else {
			node.children = nullptr;
		}


		return node;
	}
} NodeBuilder;



int fillGraph(
	int frontDist, int sideDist,
	int range,
	bool addEmptyChild,
	Game* game,
	std::vector<PriorityNode>& priorities,
	Spy spy
) {
	auto cell = game->getCell(spy.x, spy.y);
	
	
	// Check road
	if (cell->hasCar()) {
		Car* car = game->getCar(spy.x, spy.y);
		
	}


	if (range <= 0)
		return -1;

	std::vector<int> children;

	// Adapt to road
	switch (cell->getType()) {
	case CellType::NONE:
		return -1;

	// case CellType::YIELD:
		if (addEmptyChild)
			break;
		// continue to ROAD behavior	

	case CellType::ROAD:
	{
		// check to the right
		Spy rspy = spy;
		rspy.dir = Direction_getRight(spy.dir);
		rspy.move();

		int rnode = fillGraph(frontDist, sideDist+1,
			range-1, false, game, priorities, rspy);

		if (rnode >= 0) {
			children.push_back(rnode);
		}

		
		// Check to the left
		Spy lspy = spy;
		lspy.dir = Direction_getRight(spy.dir);
		lspy.move();

		int lnode = fillGraph(frontDist, sideDist+1,
			range-1, false, game, priorities, lspy);

		if (lnode >= 0) {
			children.push_back(lnode);
		}

		
		// Check in front
		spy.move();
		int fnode = fillGraph(frontDist, sideDist+1,
			range-1, false, game, priorities, spy);

		if (fnode >= 0) {
			children.push_back(fnode);
		}
		
		

		break;
	}
	
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
		car->getSpeed(), node->other->getSpeed(),
		node->other->getAcceleration(),
		car->speedLimit, node->other->speedLimit,
		carExitDist, sideEntryDist
	);

	if (fastAcc == INFINITY_F)
		fastAcc = -INFINITY_F;

	
	// Needs to wait the car with the priority
	float slowAcc = computeAcceleration(
		car->getSpeed(), node->other->getSpeed(),
		node->other->getAcceleration(),
		car->speedLimit, node->other->speedLimit,
		carEntryDist, sideExitDist
	);

	// if (slowAcc == INFINITY_F) // is this case even possible?
		// slowAcc = maxAcceleration;


	// Pass BEFORE the car (so try to use fastAcc)
	if (fastAcc <= maxAcceleration) {
		float min = maxAcceleration;
		mfor(node->children, node->childrenLength, c) {
			float acc = getNodeAcc(car, maxAcceleration,
				priorities, &priorities[*c]);

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
	mfor(node->children, node->childrenLength, c) {
		float acc = getNodeAcc(car, maxAcceleration,
			priorities, &priorities[*c]);

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
			
			int previousNodeIdx = priorities.size() - 1;
			int nodeIdx = fillGraph(dist, 0, SIDE_RANGE - dist,
				true, game, priorities, checker);

			if (nodeIdx >= 0 && previousNodeIdx >= 0) {
				PriorityNode& node = priorities[nodeIdx];
				node.children[node.childrenLength-1] = previousNodeIdx;
				node.childrenLength++;
			}
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




	priorities.clear();
	return {
		maxAcceleration,
		targetPoint
	};
}
