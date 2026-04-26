#include "Server.hpp"

#include "declarations.hpp"

#include "MISSED_REGION_SIZE.hpp"
#include "ClientId.hpp"
#include "hash.hpp"
#include "Pool.hpp"
#include "Client.hpp"
#include "Match.hpp"

#include <game/Game.hpp>
#include <game/Map.hpp>
#include <game/runGameCommand.hpp>
#include <game/entities_helper.hpp>


#include <iostream>
#include <vector>
#include <stdint.h>
#include <cmath>



Server::Server(int poolNum):
	poolNum(poolNum),
	pools(std::make_unique<Pool[]>(poolNum))
{}




#define take(T) ({ T _v = *(T*)ptr; ptr = (uint8_t*)ptr + sizeof(T); _v; })
#define push(T, val) {*(T*)res = (T)val; res += sizeof(T);}
#define align(ptr,n) {ptr += n;}
#define retRes() {*(uint32_t*)response =  (uint32_t)(res - response - 4); return response;}



uint8_t* Server::connect(Client* client, const uint8_t* ptr) {
	static constexpr int BUFFER_LENGTH = 4+ 4+4+4+8;

	uint8_t* const response = (uint8_t*)malloc(BUFFER_LENGTH);
	uint8_t* res = response;
	align(res,4); // for final size

	
	Match* match;
	align(ptr,7);
	hash_t hash = take(hash_t);

	if (hash == 0) {
		// Create match
		push(uint8_t, ClientId::JOIN_CREATED);
		align(res,3);
		push(uint32_t, MISSED_REGION_SIZE);
		align(res,4);

		hash = hash_generate();
		match = this->createMatch(hash);
	
	} else {
		push(uint8_t, ClientId::JOIN_ALIVE);
		align(res,3);
		push(uint32_t, MISSED_REGION_SIZE);
		align(res,4);

		match = this->getMatch(hash);
	}

	push(hash_t, hash);

	if (!match) {
		throw std::runtime_error{"Match not found"};
	}

	match->pushClient(client);
	retRes();
}

int clamp(int v, int minV, int maxV) {
	return std::max(minV, std::min(v, maxV));
}

uint8_t* Server::listen(Client* client, const uint8_t* ptr) {
	if (!client->match)
		throw std::runtime_error("No match to listen");

	Match* match = client->match;

	// TODO: fill missedRegions
	std::vector<uint64_t> missedRegions;


	align(ptr,3);

	int32_t x = take(int32_t);
	int32_t y = take(int32_t);
	int32_t w = take(int32_t);
	int32_t h = take(int32_t);



	// Compute region bounds
	int rx0 = (int)std::floor(
		(float)clamp(x, match->mapX, match->mapX + match->mapW - 1)
		/ MISSED_REGION_SIZE
	);

	int ry0 = (int)std::floor(
		(float)clamp(y, match->mapY, match->mapY + match->mapH - 1)
		/ MISSED_REGION_SIZE
	);

	int rx1 = (int)std::floor(
		(float)clamp(x + w - 1, match->mapX, match->mapX + match->mapW - 1)
		/ MISSED_REGION_SIZE
	);

	int ry1 = (int)std::floor(
		(float)clamp(y + h - 1, match->mapY, match->mapY + match->mapH - 1)
		/ MISSED_REGION_SIZE
	);

	// Collect new regions
	struct Region {
		uint64_t key;
		int32_t rx, ry;
	};

	std::vector<Region> newRegions;

	for (int ry = ry0; ry <= ry1; ++ry) {
		for (int rx = rx0; rx <= rx1; ++rx) {
			uint64_t key = (uint64_t(rx) << 32) | uint32_t(ry);
			if (match->visitedPoints.find(key) == match->visitedPoints.end()) {
				newRegions.push_back({key, rx, ry});
			}
		}
	}

	// missed regions inside view
	std::vector<Region> missedInView;

	for (uint64_t key : missedRegions) {
		int32_t rx = int32_t(key >> 32);
		int32_t ry = int32_t(uint32_t(key));
		missedInView.push_back({key, rx, ry});
	}

	// total count
	auto totalCount = uint32_t(newRegions.size() + missedInView.size());

	uint8_t* const response = (uint8_t*)malloc(
		+4            // final size
		+4            // action code + padding
		+4            // totalCount
		+totalCount*( // content
			4+4+
			2*MISSED_REGION_SIZE*MISSED_REGION_SIZE
		)
	);
	uint8_t* res = response;
	align(res,4); // for final size
	push(uint8_t, ClientId::AREAS);
	align(res,3);
	push(uint32_t, totalCount);

	auto game = match->getGame();

	// Send new (unvisited) regions
	for (auto r: newRegions) {
		push(int32_t, r.rx);
		push(int32_t, r.ry);
		game->map.copyCells(
			(Cell*)res,
			r.rx * MISSED_REGION_SIZE,
			r.ry * MISSED_REGION_SIZE,
			MISSED_REGION_SIZE,
			MISSED_REGION_SIZE
		);

		// move
		res += MISSED_REGION_SIZE*MISSED_REGION_SIZE*2;

		client->visitedRegions.insert(r.key);
	}

	// Send missed regions (and remove them from the set)
	for (auto r: missedInView) {
		push(int32_t, r.rx);
		push(int32_t, r.ry);
		game->map.copyCells(
			(Cell*)res,
			r.rx * MISSED_REGION_SIZE,
			r.ry * MISSED_REGION_SIZE,
			MISSED_REGION_SIZE,
			MISSED_REGION_SIZE
		);

		// move
		res += MISSED_REGION_SIZE*MISSED_REGION_SIZE*2;
	}



	// update view
	client->viewX = x;
	client->viewY = y;
	client->viewW = w;
	client->viewH = h;

	retRes();
}


uint8_t* Server::runCommand(Client* client, const uint8_t* ptr) {
	Match* match = client->match;
	Game& game = *match->getGame();

	auto count = take(uint8_t);
	ptr = (uint8_t*)runGameCommand(game, ptr);
	
	for (uint8_t i = 1; i < count; i++) {
		align(ptr,2);
		ptr = (uint8_t*)runGameCommand(game, ptr);
	}

	return nullptr;
}

uint8_t* Server::getEntities(Client* client, const uint8_t* ptr) {
	Match* match = client->match;
	Game& game = *match->getGame();

	


	uint32_t* msg = entities_helper_make(
		game,
		client->viewX,
		client->viewY,
		client->viewW,
		client->viewH,
		(uint8_t)ClientId::GET_ENTITIES
	);

	uint32_t fullSize = msg[1];
	printf("fullSize %d\n", fullSize);

	client->send(msg, fullSize + (uint32_t)sizeof(uint32_t));

	return nullptr;

}

uint8_t* Server::onerror(Client* client, const uint8_t* ptr) {
	return nullptr;
}


Match* Server::createMatch(hash_t hash) {
	Pool* pool = &this->pools[hash % this->poolNum];

	auto match = pool->createMatch(hash);
	return match;
}

void Server::deleteMatch(hash_t hash) {
	Pool* pool = &this->pools[hash % this->poolNum];
	pool->deleteMatch(hash);
}

Match* Server::getMatch(hash_t hash) {
	Pool* pool = &this->pools[hash % this->poolNum];
	return pool->getMatch(hash);
}
