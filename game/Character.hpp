#pragma once

#include "declarations.hpp"
#include "Vector.hpp"
#include "ActionExecutor.hpp"

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

public:
	float x;
	float y;

	static Character* createClientCharacter(float x, float y);


	bool makeWalk(Game& game, int destX, int destY);
	bool makeDrive(Game& game, int destX, int destY);
	bool makeInside(Game& game);

	void notifyDrive();

	void frame(Game& game);

	CharacterState getState() const;

	~Character();
};