#pragma once

#include "declarations.hpp"
#include <game/hash.hpp>
#include "Client.hpp"
#include "GameOwner.hpp"

#include <vector>

#include "game/calendar_t.hpp"

struct Match {
private:
	Game* const game;
	calendar_t nextAutoSave;

public:
	hash_t const hash;
	Pool* const pool;
	std::vector<Client*> clients;
	int mapX = 0;
	int mapY = 0;
	int mapW = 32;
	int mapH = 32;

	int pushClient(Client* client);
	bool popClient(Client* client);

	template<bool serv>
	GameOwner<serv> getGame();

	bool checkAutoSave();

	Match(Pool* pool, hash_t hash);
	~Match();
};
