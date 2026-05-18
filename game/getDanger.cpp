#include "getDanger.hpp"

#include "utils/mfor.hpp"
#include "utils/mergeSortedVectors.hpp"

#include "Car.hpp"
#include "PathHandler.hpp"
#include "Game.hpp"
#include "Cell.hpp"
#include "debugFile.hpp"
#include "PriorityNode.hpp"
#include "Direction.hpp"

#include <limits>
#include <vector>
#include <math.h>
#include <set>

#include "carstats.hpp"


using namespace carstats;

static constexpr float INFINITY_F = std::numeric_limits<float>::infinity();

/**
 * @param inf Value returned if acceleration is not at stake
 */
float computeAcceleration(
	float vx0, float vy0,
	float a_y,
	float vx_max, float vy_max,
	float X, float Y, float inf
) {
	a_y=0; // it created bugs because a_y was alterning

	// Handle already inside cases
	if (X<0 || Y<0) {
		debugLog("revert %f %f\n", inf, -inf);
		return -inf; // stop
	}

	if (X<0)
		return inf; // accelerate

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
		if (vy0 <= 0.0f) return inf;
		T = Y / vy0;

	} else { // a_y < 0
		float discriminant = vy0 * vy0 + 2.0f * a_y * Y;
		if (discriminant < 0.0f) return inf;

		float T_before = (-vy0 + std::sqrt(discriminant)) / a_y;

		if (T_before <= t_star) {
			T = T_before;
		} else {
			float y_max = vy0 * t_star + 0.5f * a_y * t_star * t_star;
			if (Y > y_max) return inf;
			T = t_star;
		}
	}

	// Avoid division by zero
	if (T <= 0.0f) return inf;


	// Compute candidate a_x without x saturation
	float a_x_noSat = 2.0f * (X - vx0 * T) / (T * T);
	debugLog("axNoSat %.3f\n", a_x_noSat);


	if (a_x_noSat > 0.0f) {
		
		// Fix saturation
		if (vx0 + a_x_noSat * T > vx_max) {
			float maxDist = vx_max*T - X;

			// Check if even instantly at maxSpeed,
			// we can't pass at time (car too slow)
			if (maxDist <= 0) {
				return +INFINITY_F;
			}

			float n = vx_max - vx0;
			return 0.5f * n*n / maxDist;
		}

	} else if (a_x_noSat < 0.0f) {
		// Fix saturation
		if (vx0 + a_x_noSat * T < 0.0f) {
			return -vx0/T;
		}
	}

	return a_x_noSat;
}

struct Spy {
	int x;
	int y;
	Direction dir;

	void move() {
		this->x += Direction_getVector(this->dir).x;
		this->y += Direction_getVector(this->dir).y;
	}

	bool operator<(const Spy& other) const {
		if (x != other.x) return x < other.x;
		if (y != other.y) return y < other.y;
		return dir < other.dir;
	}
};


/**
 *
 * @param firstIteration If true, then add empty child
 */
static int fillGraph(
	int frontDist, int sideDist,
	int range,
	bool firstIteration,
	Game* game,
	std::vector<PriorityNode>& priorities,
	Spy spy,
	std::set<Spy>& visited
) {
	if (range <= 0)
		return -1;
		
	
	if (visited.contains(spy))
		return -1;

	visited.insert(spy);

	auto cell = game->getCell(spy.x, spy.y);


	const Direction opposedSpyDir = Direction_getOpposite(spy.dir);
	// Adapt to road
	switch (cell->getType()) {
	case CellType::NONE:
		return -1;

	case CellType::INSTRUCTION: {
		auto type = ((cell->data >> 4) & 0x3);
		switch (type) {
			// yield
			case 0: {
				Direction dir = (Direction)((cell->data >> 6) & 0x3);
				if (dir == opposedSpyDir && firstIteration) {
					return -1;
				}

				goto checkRoad;
			}

			default:
				throw std::runtime_error{"TODO CellType::INSTRUCTION"};

		}
	}

	case CellType::LIGHT: {
		Direction dir = (Direction)((cell->data >> 12) & 0x3);
		if (
			dir == opposedSpyDir &&
			(cell->data & (1 << (4 + game->getLightCooldown()))) == 0
		) {
			// light is red, so we can ignore this priority
			return -1;
		}

		goto checkRoad;
	}


	case CellType::ROAD:
	checkRoad:
	{
		int children[3];
		int childrenNodeIdx = -1;

		Car* car;
		if (cell->hasCar()) {
			car = game->getCar(spy.x, spy.y);
			if (car->direction != opposedSpyDir)
				car = nullptr;

		} else {
			car = nullptr;
		}



		{
			// Check in front
			Spy fspy = spy;
			fspy.move();
			children[0] = fillGraph(frontDist, sideDist+1,
				range-1, false, game, priorities, fspy, visited);
	
	
			// check to the right
			Spy rspy = spy;
			rspy.dir = Direction_getRight(spy.dir);
			rspy.move();
	
			children[1] = fillGraph(frontDist, sideDist+1,
				range-1, false, game, priorities, rspy, visited);
	
			
			// Check to the left
			Spy lspy = spy;
			lspy.dir = Direction_getRight(spy.dir);
			lspy.move();
	
			children[2] = fillGraph(frontDist, sideDist+1,
				range-1, false, game, priorities, lspy, visited);
		}

		

		int validLength = 0;
		for (int i = 0; i < 3; i++)
			if (children[i] != -1)
				validLength++;
		

		if (validLength == 1 && !car) {
			for (int i = 0; true; i++) {
				if (children[i] >= 0) {
					return children[i];
				}
			}
		}


		if (validLength == 0 && !car)
			return -1;

		int fullValid = validLength + (firstIteration?0:1);
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
	}


	default:
		break;
	
	}


	return -1;
}


typedef struct {
	float slow;
	float fast;
} NodeAcc;

NodeAcc getNodeAcc(
	const Car* car,
	float maxAcceleration,
	std::vector<PriorityNode>& priorities,
	PriorityNode* node
) {
	const Car* other = node->car;
	float carEntryDist = (float)node->frontDist - car->step +
		(1 - Car::WIDTH/2);

	float carExitDist = (float)node->frontDist - car->step + 2 + (Car::WIDTH/2);

	float sideEntryDist = (float)node->sideDist - other->step +
		(1 - Car::WIDTH/2);

	float sideExitDist = (float)node->sideDist - other->step + 2 + Car::WIDTH/2;


	debugLog("[y=%02d] ; entryDist=%.3f ; exitDist=%.3f sideEntryDist=%.3f"
		"sideExitDist=%.3f speed=%.3f otherSpeed=%.3f otherAcc=%.3f\n",
		other->y, carEntryDist, carExitDist, sideEntryDist, sideExitDist,
		car->getSpeed(), other->getSpeed(), other->getAcceleration());


	debugLog("fastAcc:\n");
	float fastAcc = computeAcceleration(
		car->getSpeed(), other->getSpeed(),
		other->getAcceleration(),
		car->speedLimit, other->speedLimit,
		carExitDist, sideEntryDist, -INFINITY_F
	);
	debugLog("fastAcc=%f\n", fastAcc);
	
	

	
	debugLog("slowAcc:\n");
	// Needs to wait the car with the priority
	float slowAcc = computeAcceleration(
		car->getSpeed(), other->getSpeed(),
		other->getAcceleration(),
		car->speedLimit, other->speedLimit,
		carEntryDist, sideExitDist, INFINITY_F
	);
	debugLog("slowAcc=%f\n", slowAcc);



	if (slowAcc == INFINITY_F) // is this case even possible?
		slowAcc = maxAcceleration;
	

	// Check for the car and its children
	NodeAcc bounds = {.slow = slowAcc, .fast = fastAcc};
	for(int* cptr = node->children; true; cptr++) {
		int c = *cptr;
		if (c < 0)
			break;

		NodeAcc acc = getNodeAcc(car, maxAcceleration,
			priorities, &priorities[c]);


		if (acc.slow < bounds.slow)
			bounds.slow = acc.slow;

		if (acc.fast > bounds.fast)
			bounds.fast = acc.fast;
	}


	debugLog("[y=%02d] bslow=%.3f bfast=%.3f s=%.3f f=%.3f\n",
		other->y, bounds.slow, bounds.fast, slowAcc, fastAcc);
	return bounds;
}


getDanger_t getDanger(
	const Car* car,
	Game* game,
	std::vector<PriorityNode>& priorities
) {
	enum {
		FRONT_RANGE = 64,
		SIDE_RANGE = 64
	};


	auto pathHandler = PathHandler<false>{car->pathHandler};

	float maxAcceleration = MAX_ACCELERATION;
	const float speedLimit = car->speedLimit;
	const float carSpeed = car->getSpeed();
	const float carSpeed2 = carSpeed * carSpeed;
	Vector<int> targetPoint{-1, -1};

	const auto appendStopDist = [
		carSpeed, carSpeed2, &targetPoint,
		speedLimit, &maxAcceleration, car
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
			float acc = (speedLimit - carSpeed) * (1.0f/SPEED_FACTOR);
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
	const Direction spyOpposedDir = Direction_getOpposite(car->direction);
	Vector<int> pathPoint = pathHandler.seek();
	int firstNodeIdx = -1;
	int previousNodeIdx = -1;

	
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
			case CellType::ROAD:
			case CellType::PARKING: {
				checkRightPriority = true;
				checkLeftPriority = false;

				defaultDetection:
				if (other == nullptr || other == car)
					break;

				Direction otherDirection = other->direction;
				float stopDist;

				if (otherDirection == spy.dir) {
					float shrinkedStep = other->step - Car::WIDTH/2;
					if (shrinkedStep > 0)
						shrinkedStep = 0;

					stopDist = (float)dist + shrinkedStep - car->step - Car::WIDTH/2;

				} else if (otherDirection == spyOpposedDir) {
					throw std::runtime_error{"TODO: otherDirection == spyOpposedDir"};
					stopDist = 10e6f;

				} else { // Side direction
					stopDist = (float)dist - car->step + (
						(1-Car::HEIGHT)/2 - Car::WIDTH/2);

				}

				appendStopDist(
					stopDist,
					FRONT_DECELERATION,
					{spy.x, spy.y}
				);

				break;
			}

			case CellType::INSTRUCTION: {
				auto type = ((cell->data >> 4) & 0x3);
				switch (type) {
					// yield
					case 0: {
						auto dir = (Direction)((cell->data >> 6) & 0x3);
						if (car->direction == dir) {
							checkRightPriority = true;
							checkLeftPriority = true;
						} else {
							checkRightPriority = true;
							checkLeftPriority = false;

						}
						goto defaultDetection;
					}

					default:
						throw std::runtime_error{"TODO CellType::INSTRUCTION"};

				}

				break;
			}

			case CellType::LIGHT: {
				if ((Direction)((cell->data >> 12) & 0x3) != spy.dir)
					goto defaultDetection; // ignore light

				if (cell->data & (1 << (4 + game->getLightCooldown()))) {
					// light is green
					checkRightPriority = true;
					checkLeftPriority = false;
					goto defaultDetection;
				} else {
					// light is red
					goto stopAtCell;
				}

				break;
			}

			default: {
				stopAtCell:
				appendStopDist(
					(float)dist - car->step - Car::WIDTH/2,
					SOFT_DECELERATION,
					{spy.x, spy.y}
				);
				goto finishUpdate;
			}

		}



		auto checkPriority = [&](Direction checkerDir) {
			Spy checker{spy};

			checker.move();
			checker.dir = checkerDir;
			checker.move();

			std::set<Spy> visited;
			int nodeIdx = fillGraph(dist, 0, SIDE_RANGE - dist,
				true, game, priorities, checker, visited);


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

				} else {
					firstNodeIdx = nodeIdx;
				}

				previousNodeIdx = nodeIdx;
			}
		};


		if (dist >= SIDE_RANGE)
			goto skipPriorities;

		// Check right priority
		if (checkRightPriority) {
			checkPriority(Direction_getRight(spy.dir));
		}

		// Check left priority
		if (checkLeftPriority) {
			checkPriority(Direction_getLeft(spy.dir));
		}




		skipPriorities:


	
		// Check if we need to turn
		if (spy.x == pathPoint.x && spy.y == pathPoint.y) {
			spy.dir = pathHandler.seekDirection();
			// Move and check if path is finished
			if (!pathHandler.next()) {
				appendStopDist(
					(float)dist - car->step + (1 - Car::WIDTH/2),
					SOFT_DECELERATION,
					{spy.x, spy.y}
				);
				goto finishUpdate;


			}
			pathPoint = pathHandler.seek();

		}

		// Move
		spy.move();
	}



	



	finishUpdate:

	float finalAcc;

	// Get priority acceleration
	if (firstNodeIdx >= 0) {
		NodeAcc bounds = getNodeAcc(car, maxAcceleration,
			priorities, &priorities[firstNodeIdx]);

		finalAcc = bounds.fast > maxAcceleration ?
			bounds.slow :
			maxAcceleration;

	} else {
		finalAcc = maxAcceleration;
	}



	for (auto i: priorities)
		free(i.children);

	priorities.clear();

	return {
		finalAcc,
		targetPoint
	};
}
