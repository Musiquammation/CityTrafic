#pragma once

#include "declarations.hpp"

enum class CharacterState {
	CLIENT,
	WALK,
	INSIDE,
	DRIVE
};

class Character {
	CharacterState state;
	Car* car = nullptr;

	Character() = default;

	union {
		struct {

		} client;

		struct {
			PathHandler<true>* path;
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

	CharacterState getState() const;

	~Character();
};