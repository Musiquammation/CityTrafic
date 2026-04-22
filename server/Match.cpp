#include "Match.hpp"

#include <algorithm>
#include <game/Game.hpp>

Match::Match(): game(new Game{}) {}
Match::~Match() {
    delete this->game;
}


int Match::pushClient(Client* client) {
    this->clients.push_back(client);
    client->match = this;
    return (int)this->clients.size() - 1;
}

bool Match::popClient(Client* client) {
    auto it = std::find(this->clients.begin(), this->clients.end(), client);
    if (it != this->clients.end()) {
        this->clients.erase(it);
        return true;
    }

    return false;
}