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
		this->y += Direction_getVector(this->dir).x;
	}

} Spy;

getDanger_t getDanger(const Car* car, Game* game) {
    enum {
		FRONT_RANGE = 32
	};

	static constexpr float INFINITY = 
		std::numeric_limits<float>::infinity();

    auto pathHandler = PathHandler<false>{car->pathHandler};

	Spy spy{car->x, car->y, car->direction};
	Vector<int> pathPoint = pathHandler.seek();
	float maxSpeed = INFINITY;

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
		
		// Handle cell
		switch (cellType) {
		case CellType::NONE:
			goto finishUpdate;


		}


		// Check right priority


		
	}


	finishUpdate:

	


	return {
        maxSpeed
    };
}
