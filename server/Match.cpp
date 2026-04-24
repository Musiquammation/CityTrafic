#include "Match.hpp"

#include <algorithm>
#include <game/Game.hpp>

Match::Match(Pool* pool, hash_t hash):
    game(new Game{}),
    hash(hash),
    pool(pool)
{}

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

template<bool serv>
GameOwner<serv> Match::getGame() {
    return GameOwner<serv>{this->game, this->pool, this->hash};
}

template GameOwner<false> Match::getGame<false>();
template GameOwner<true> Match::getGame<true>();