#pragma once

#include "declarations.hpp"
#include "hash.hpp"
#include "Client.hpp"
#include "GameOwner.hpp"

#include <vector>
#include <set>

struct Match {
private:
	Game* const game;

public:
	hash_t const hash;
	Pool* const pool;
	std::vector<Client*> clients;
	int mapX = 0;
	int mapY = 0;
	int mapW = 32;
	int mapH = 32;
	std::set<uint64_t> visitedPoints;

	int pushClient(Client* client);
	bool popClient(Client* client);

	template<bool serv = false>
	GameOwner<serv> getGame();

	Match(Pool* pool, hash_t hash);
	~Match();
};