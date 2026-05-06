#include "CharacterHandler.hpp"

void CharacterHandler::pushCharacter(Character* character) {
	this->characters.push_back(character);
}

bool CharacterHandler::contains(Character* character) {
	for (auto c: this->characters) {
		if (c == character)
			return true;
	}
	return false;
}