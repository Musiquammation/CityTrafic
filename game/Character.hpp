#pragma once

#include "declarations.hpp"
#include "Vector.hpp"
#include "ActionExecutor.hpp"

namespace actionNodes::character {
	struct CharacterFriend;
}

enum class CharacterState {
	CLIENT,
	WALK,
	INSIDE,
	DRIVE
};

class Character {
	CharacterState state;
	Car* car = nullptr;
	ActionExecutor executor;
	Vector<int> home;
	int homePosition;
	unsigned int pointId;


	static constexpr float FRONT_SPEED = .04f;
	static constexpr float SIDE_SPEED = FRONT_SPEED/1.4f;

	Character();

	union {
		struct {

		} client;

		struct {
			char* path;
			int position;
		} walk;

		struct {
			
		} inside;

		struct {
			
		} drive;
	} data;

	void cleanupState();
	void setState(CharacterState next);

	friend struct actionNodes::character::CharacterFriend;

public:
	float x;
	float y;

	static Character* createClientCharacter(float x, float y);
	static Character* spawnCharacter(const Map& map, int x, int y);

	bool makeWalk(Game& game, int destX, int destY);
	bool makeDrive(Game& game, int destX, int destY);
	bool makeInside(Game& game);

	void notifyDrive();

	void frame(Game& game);
	int takeRandomPointId(int modulo);

	CharacterState getState() const;

	~Character();
};