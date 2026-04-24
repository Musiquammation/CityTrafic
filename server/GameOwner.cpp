#include "GameOwner.hpp"

#include "Pool.hpp"
#include <stdio.h>

template<bool poolThread>
GameOwner<poolThread>::GameOwner(Game* game, Pool* pool, hash_t hash)
	: game(game), pool(pool)
{
	pool->acquireGame(hash, poolThread ? Pool::POOL_ID : Pool::SERVER_ID);
}


template<bool poolThread>
GameOwner<poolThread>::~GameOwner() {
	this->pool->releaseGame(poolThread ? Pool::POOL_ID : Pool::SERVER_ID);
}




template<bool serv>
Game* GameOwner<serv>::operator->() {
	return this->game;
}

template<bool serv>
const Game* GameOwner<serv>::operator->() const {
	return this->game;
}

template<bool serv>
Game& GameOwner<serv>::operator*() {
	return *this->game;
}

template<bool serv>
const Game& GameOwner<serv>::operator*() const {
	return *this->game;
}



template class GameOwner<false>;
template class GameOwner<true>;