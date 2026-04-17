#pragma once

#include <vector>
#include "declarations.hpp"

class CharacterHandler {
    friend class Api;
    std::vector<Character*> characters;
};