#pragma once


enum class CharacterState {
	WALK,
	BUILDING,
	DRIVE
};

class Character {
public:
	float x;
	float y;
	CharacterState state;
};