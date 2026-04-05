#pragma once

#include "declarations.hpp"
#include "Game.hpp"

typedef struct {
    Game game;
} Api;



extern "C" {
    Api* Api_create();
    void Api_delete(Api* api);
    void Api_reserve(Api* api, int code);
}
