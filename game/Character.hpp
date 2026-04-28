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
	CharacterState state;
	Car* car = nullptr;
	ActionExecutor executor;
	Vector<int> home;
	int homePosition;
	unsigned int pointId;


	static constexpr float SPEED = .04f;

	Character();

	union {
		struct {

		} client;

		struct {
			char* path;
			int position;
			float step;
			Vector<int> anchor;
		} walk;

		struct {
			
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

	void notifyDrive();

	BuildingInfo getWorkBuilding(const Map& map) const;
	BuildingInfo getHomeBuilding(const Map& map) const;
	bool orientBuilding(Game& game, BuildingInfo info);
	bool locateBuilding(Map& map, BuildingInfo info);

	void frame(Game& game);
	int takeRandomPointId(int modulo);

	CharacterState getState() const;

	Car* getCar() const;
	bool setCar(Car* car);

	~Character();
};