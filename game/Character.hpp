#pragma once


enum class CharacterState {
	WALK,
	BUILDING,
	DRIVE
};

class Character {
public:
	int x;
	int y;
	CharacterState state;
};