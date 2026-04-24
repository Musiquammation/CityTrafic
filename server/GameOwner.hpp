#pragma once

#include "declarations.hpp"
#include "hash.hpp"

template<bool poolThread>
class GameOwner {
	Game* game;
	Pool* pool;

public:
	GameOwner(Game* game, Pool* pool, hash_t hash);
	~GameOwner();
	Game* operator->();
	const Game* operator->() const;
	Game& operator*();
	const Game& operator*() const;
};


