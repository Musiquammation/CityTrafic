#pragma once

#include "declarations.hpp"
#include "Vector.hpp"
#include "BuildingInfo.hpp"
#include "ActionExecutor.hpp"
#include "actions/ActionCode.hpp"

namespace actionNodes::character {
	struct CharacterFriend;
}

enum class CharacterState {
	CLIENT,
	WALK,
	INSIDE,
	OUTSIDE,
	DRIVE
};

class Character {
	Car* car = nullptr;
	ActionExecutor executor;
	Job* job = nullptr;
	Vector<int> home;
	CharacterState state;
	int money = 0;
	int waitingJoyMoney = 0;
	unsigned int pointId;


	static constexpr float SPEED = .04f;

	Character();

	union {
		struct {

		} client;

		struct {
			char* path;
			Vector<int> anchor;
			int position;
			float step;
		} walk;

		struct {
			int index;
		} inside;

		struct {

		} outside;

		struct {
			ActionCode state;
		} drive;
	} data;

	void cleanupState();
	void setState(CharacterState next);
	ActionCode walk(Game& game);

	friend struct actionNodes::character::CharacterFriend;

public:
	float x;
	float y;

	static Character* createClientCharacter(float x, float y);
	static Character* spawnCharacter(const Map& map, int x, int y);

	bool makeWalk(Game& game, int destX, int destY);
	bool makeDrive(Map& map, int destX, int destY);
	bool makeInside(Game& game);
	void makeOutside(Game& game);

	void notifyDrive();

	BuildingInfo getWorkBuilding(Game& game) const;
	BuildingInfo getHomeBuilding(const Map& map) const;
	bool orientBuilding(Game& game, BuildingInfo info);
	bool locateBuilding(Map& map, BuildingInfo info);

	void frame(Game& game);
	int takeRandomPointId(int modulo);

	CharacterState getState() const;

	Car* getCar() const;
	bool setCar(Car* car);

	bool takeJob(Job* job, const Calendar& calendar);
	void leaveJob();
	Job* getJob();

	~Character();
};