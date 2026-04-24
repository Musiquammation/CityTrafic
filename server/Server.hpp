#pragma once

#include "declarations.hpp"
#include "clientId_t.hpp"

#include "Pool.hpp"


#include <memory>
#include <unordered_map>

class Server {
	int poolNum;
	clientId_t nextId = 0;
	std::unique_ptr<Pool[]> pools;    
	std::unordered_map<clientId_t, Client*> clients;

	uint8_t* connect(Client* client, const uint8_t* args);
	uint8_t* listen(Client* client, const uint8_t* args);
	uint8_t* runCommand(Client* client, const uint8_t* args);
	uint8_t* getEntities(Client* client, const uint8_t* args);
	uint8_t* onerror(Client* client, const uint8_t* args);

	Match* createMatch(hash_t hash);
	void deleteMatch(hash_t hash);
	Match* getMatch(hash_t hash);

public:
	Server(int poolNum);
	void run(int port);
};