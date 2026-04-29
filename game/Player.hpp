#pragma once

#include <game/hash.hpp>


struct Player {
    hash_t password = hash_generate();
    int money = 0;
};