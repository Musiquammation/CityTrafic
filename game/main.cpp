#include "api.hpp"
#include "Game.hpp"

#include <stdio.h>

int main() {
    auto api = Api_create();

    printf("Hello %p\n", api);

    Api_delete(api);

    return 0;
}