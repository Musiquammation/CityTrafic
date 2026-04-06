#pragma once

#include "declarations.hpp"
#include "Game.hpp"

struct Api {
    Game game;
};



extern "C" {
    Api* Api_create();
    void Api_delete(Api* api);
    void Api_frame(Api* api);
    void* Api_take(Api* api, int code);
}
