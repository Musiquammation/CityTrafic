#pragma once

#include "declarations.hpp"
#include "Vector.hpp"
#include "Direction.hpp"
#include "PathHandler.hpp"

#include <stdint.h>

enum class CarState: uint8_t {
	FRONT,
	TURN_RIGHT,
	TURN_LEFT,
};



class Car {

public:
	static constexpr float MAX_DECELERATION = .01f;
	static constexpr float SOFT_DECELERATION = .003f;
	static constexpr float MAX_ACCELERATION = .005f;
	static constexpr int SPEED_FACTOR = 30;
	static constexpr float WIDTH = .9f;
	static constexpr float HEIGHT = .6f;

	int x;
	int y;
	float step = 0.5;
	float speed = 0;
	float speedLimit = 0.4f;
	
	PathHandler<true> pathHandler;
	Direction direction;
	CarState state = CarState::FRONT;

	Car(int x, int y, Direction direction);

	void update(Game* game);
	void move();

	Vector<float> calcPosition() const;
};