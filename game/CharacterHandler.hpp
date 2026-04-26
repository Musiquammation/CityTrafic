#pragma once

#include <vector>
#include "declarations.hpp"

class CharacterHandler {
	friend class Api;
	std::vector<Character*> characters;

public:
	void pushCharacter(Character* character);

	auto begin() { return this->characters.begin(); }
	auto end() { return this->characters.end(); }
	auto begin() const { return this->characters.begin(); }
	auto end() const { return this->characters.end(); }

};