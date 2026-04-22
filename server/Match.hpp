#pragma once

#include "declarations.hpp"
#include "Client.hpp"

#include <vector>
#include <set>

struct Match {
	Game* const game;
	std::vector<Client*> clients;
	int mapX = 0;
	int mapY = 0;
	int mapW = 32;
	int mapH = 32;
	std::set<uint64_t> visitedPoints;

	int pushClient(Client* client);
	bool popClient(Client* client);

	Match();
	~Match();
};