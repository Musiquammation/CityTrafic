#pragma once

#include "declarations.hpp"
#include "Vector.hpp"
#include "Direction.hpp"
#include "PathHandler.hpp"

#include <stdint.h>
#include <vector>

enum class CarState: uint8_t {
	FRONT,
	TURN_RIGHT,
	TURN_LEFT,
};



class Car {
private:
	Vector<int> realTargetPoint;
	Vector<int> publicTargetPoint{-1, -1};
	float publicAcceleration = 0;
	float realSpeed = 0;
	float publicSpeed = 0;
	Character* driver = nullptr;

	friend class Api;

public:
	static constexpr float SOFT_DECELERATION = .002f;
	static constexpr float FRONT_DECELERATION = .009f;
	static constexpr float MAX_ACCELERATION = .02f;
	static constexpr int SPEED_FACTOR = 30;
	static constexpr float WIDTH = .9f;
	static constexpr float HEIGHT = .6f;

	int x;
	int y;
	float step = 0.5;
	float speedLimit = 0.4f;
	
	PathHandler<true> pathHandler;
	Direction direction;
	CarState state = CarState::FRONT;

	Car(int x, int y, Direction direction);

	void update(Game* game, std::vector<PriorityNode>& prioritiesBuffer);
	void move();

	Vector<float> calcPosition() const;

	float getAcceleration() const;
	float getSpeed() const;
	Vector<int> getTargetPoint() const;

	bool appendDriver(Character* driver);
	Character* removeDriver();
};