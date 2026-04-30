#include "Server.hpp"

#include "declarations.hpp"

#include "ClientId.hpp"
#include "Pool.hpp"
#include "Client.hpp"
#include "Match.hpp"

#include <game/hash.hpp>
#include <game/Game.hpp>
#include <game/Map.hpp>
#include <game/Building.hpp>
#include <game/Job.hpp>
#include <game/runGameCommand.hpp>
#include <game/updateNet_helper.hpp>


#include <iostream>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <cmath>




#ifndef MAP_PRECISION
	#warning "MAP_PRECISION is requires for server"
	#define MAP_PRECISION 8 /* for vscode */
#endif

enum PanelVariantType {
	BUILDING,
	JOB
};

struct PanelVariant {
	PanelVariantType type;
	const uint8_t* ptr;
	union {
		Building* building;
		Job* job;
	};
};



#define take(T) ({ T _v = *(T*)ptr; ptr = (uint8_t*)ptr + sizeof(T); _v; })
#define push(T, val) {*(T*)res = (T)val; res += sizeof(T);}
#define align(ptr,n) {ptr += n;}
#define retRes() {*(uint32_t*)response =  (uint32_t)(res - response - 4); return response;}


PanelVariant getVariant(const Game& game, const uint8_t* ptr) {
	// Recognize object
	auto x = take(int32_t);
	if (x == (int)-0x80000000) {
		// Job
		auto jobIdx = take(int32_t);
		Job* job = game.getJob(jobIdx);
		if (!job) {
			throw std::runtime_error{"Cannot find job asked by panel"};
		}

		return {
			PanelVariantType::JOB,
			ptr,
			{.job = job}
		};

	}

	auto y = take(int32_t);
	auto info = game.getBuilding(x, y);
	if (!info.building) {
		throw std::runtime_error{"Cannot find building asked by panel"};
	}

	return {
		PanelVariantType::BUILDING,
		ptr,
		{.building = info.building}
	};
}



Server::Server(int poolNum):
	poolNum(poolNum),
	pools(std::make_unique<Pool[]>(poolNum))
{}







uint8_t* Server::connect(Client* client, const uint8_t* ptr) {
	static constexpr int BUFFER_LENGTH = 4+ 4+4+4+8+8;

	uint8_t* const response = (uint8_t*)malloc(BUFFER_LENGTH);
	uint8_t* res = response;
	align(res,4); // for final size

	
	Match* match;
	align(ptr,7);
	hash_t sessionHash = take(hash_t);
	hash_t playerHash = take(hash_t);

	if (sessionHash == 0) {
		// Create match
		push(uint8_t, ClientId::JOIN_CREATED);
		align(res,3);
		push(uint32_t, MAP_PRECISION);
		align(res,4);

		sessionHash = hash_generate();
		match = this->createMatch(sessionHash);
	
	} else {
		push(uint8_t, ClientId::JOIN_ALIVE);
		align(res,3);
		push(uint32_t, MAP_PRECISION);
		align(res,4);

		match = this->getMatch(sessionHash);
	}

	push(hash_t, sessionHash);

	// Add player
	{
		auto game = match->getGame<true>();
		int id = game->searchPlayer(playerHash);
		push(hash_t, game->players[id].key);
		client->playerId = id;
		client->cellsLayerId = game->map.addEditedCellsLayer();
	}

	if (!match) {
		throw std::runtime_error{"Match not found"};
	}

	match->pushClient(client);

	// Add client
	

	retRes();
}

int clamp(int v, int minV, int maxV) {
	return std::max(minV, std::min(v, maxV));
}

uint8_t* Server::listen(Client* client, const uint8_t* ptr) {
	if (!client->match)
		throw std::runtime_error("No match to listen");

	Match* match = client->match;


	align(ptr,3);

	int32_t x = take(int32_t);
	int32_t y = take(int32_t);
	int32_t w = take(int32_t);
	int32_t h = take(int32_t);



	// Compute region bounds
	int rx0 = (int)std::floor(
		(float)clamp(x, match->mapX, match->mapX + match->mapW - 1)
		/ MAP_PRECISION
	);

	int ry0 = (int)std::floor(
		(float)clamp(y, match->mapY, match->mapY + match->mapH - 1)
		/ MAP_PRECISION
	);

	int rx1 = (int)std::floor(
		(float)clamp(x + w - 1, match->mapX, match->mapX + match->mapW - 1)
		/ MAP_PRECISION
	);

	int ry1 = (int)std::floor(
		(float)clamp(y + h - 1, match->mapY, match->mapY + match->mapH - 1)
		/ MAP_PRECISION
	);

	// Collect new regions
	struct Region {
		uint64_t key;
		int32_t rx, ry;
	};

	std::vector<Region> newRegions;

	/// TODO: [optimization] loop on match->visitedPoints and check bounds
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
	auto game = match->getGame<true>();

	auto& editedCells = *(game->map.getEditLayer(client->cellsLayerId));
	for (auto it = editedCells.begin(); it != editedCells.end(); ) {
		const auto& i = *it;

		if (i.x >= rx0 && i.y >= ry0 && i.x <= rx1 && i.y <= ry1) {
			uint64_t key = (uint64_t(i.x) << 32) | uint64_t(i.y);

			if (match->visitedPoints.find(key) != match->visitedPoints.end()) {
				missedInView.push_back({key, i.x, i.y});

				it = editedCells.erase(it);
				continue;
			}
		}

		it++;
	}

	// total count
	auto totalCount = uint32_t(newRegions.size() + missedInView.size());

	uint8_t* const response = (uint8_t*)malloc(
		+4            // final size
		+4            // action code + padding
		+4            // totalCount
		+totalCount*( // content
			4+4+
			2*MAP_PRECISION*MAP_PRECISION
		)
	);
	uint8_t* res = response;
	align(res,4); // for final size
	push(uint8_t, ClientId::AREAS);
	align(res,3);
	push(uint32_t, totalCount);


	// Send new (unvisited) regions
	for (auto r: newRegions) {
		push(int32_t, r.rx);
		push(int32_t, r.ry);
		game->map.copyCells(
			(Cell*)res,
			r.rx * MAP_PRECISION,
			r.ry * MAP_PRECISION,
			MAP_PRECISION,
			MAP_PRECISION
		);

		// move
		res += MAP_PRECISION*MAP_PRECISION*2;

		client->visitedRegions.insert(r.key);
	}

	// Send missed regions (and remove them from the set)
	for (auto r: missedInView) {
		push(int32_t, r.rx);
		push(int32_t, r.ry);
		game->map.copyCells(
			(Cell*)res,
			r.rx * MAP_PRECISION,
			r.ry * MAP_PRECISION,
			MAP_PRECISION,
			MAP_PRECISION
		);

		// move
		res += MAP_PRECISION*MAP_PRECISION*2;
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
	auto game = match->getGame<true>();

	Player* player = game->getPlayer(client->playerId);
	auto count = take(uint8_t);
	
	for (uint8_t i = 0; i < count; i++) {
		ptr = (uint8_t*)runGameCommand(*game, ptr, player);
	}

	return nullptr;
}

uint8_t* Server::getUpdates(Client* client, const uint8_t* ptr) {
	Match* match = client->match;
	auto game = match->getGame<true>();

	auto player = game->getPlayer(client->playerId);
	bool updateClientJobs =
		(client->updateJobsDate != game->updateJobsDate);
	
	if (updateClientJobs) {
		client->updateJobsDate = game->updateJobsDate;
	}

	uint32_t* msg = updateNet_helper_write(
		*game,
		client->viewX,
		client->viewY,
		client->viewW,
		client->viewH,
		(uint8_t)ClientId::UPDATE,
		player->money,
		updateClientJobs
	);

	uint32_t fullSize = msg[1];

	client->send(msg, fullSize + (uint32_t)sizeof(uint32_t));

	return nullptr;

}


uint8_t* Server::getPanel(Client* client, const uint8_t* ptr) {
	Match* match = client->match;
	auto game = match->getGame<true>();

	auto writeMode = take(uint8_t);

	if (writeMode) {
		auto requestId = take(uint16_t);
		auto v = getVariant(*game, ptr);
		ptr = v.ptr;

		switch (v.type) {
		case PanelVariantType::BUILDING:
			v.building->setPanelData((uint32_t*)ptr);
			break;

		case PanelVariantType::JOB:
			v.job->setPanelData((uint32_t*)ptr);
			break;
		}


		return nullptr;
	}


	auto requestId = take(uint16_t);
	auto v = getVariant(*game, ptr);
	ptr = v.ptr;

	uint32_t* data;
	switch (v.type) {
	case PanelVariantType::BUILDING:
		data = v.building->getPanelData();
		break;

	case PanelVariantType::JOB:
		data = v.job->getPanelData();
		break;
	}

	auto msgLength = data[0];
	auto panelId = data[1];


	uint8_t* const response = (uint8_t*)malloc(
		sizeof(uint32_t) * (3+msgLength)
	);
	uint8_t* res = response;
	align(res,4); // for final size

	push(uint8_t, ClientId::PANEL);
	align(res,1);
	push(uint16_t, requestId);
	push(uint32_t, panelId);
	memcpy(res, &data[2], msgLength * sizeof(uint32_t));
	res += msgLength * sizeof(uint32_t);
	free(data);

	retRes();
}



uint8_t* Server::onerror(Client* client, const uint8_t* ptr) {
	return nullptr;
}

void Server::disconnect(Client* client) {
	auto game = client->match->getGame<true>();
	game->map.removeEditedCellsLayer(client->cellsLayerId);
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
