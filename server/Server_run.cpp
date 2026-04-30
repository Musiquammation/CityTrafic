#include "Server.hpp"

#include "Client.hpp"
#include "ServerId.hpp"

#include <uWebSockets/src/App.h>
#include <thread>
#include <stdlib.h>


void Server::run(int port) {
	// Create thread for each pool
	for (int i = 0; i < this->poolNum; i++) {
		this->pools[i].launchThread();
	}


	// Run server
	uWS::App().ws<Client>("/*", {
		/* Configuration */
		.compression = uWS::DISABLED,
		.maxPayloadLength = 16 * 1024 * 1024,
		.idleTimeout = 120,
		
		/* Handlers */
		.open = [this](auto *ws) {
			Client* client = ws->getUserData();
			client->id = this->nextId;
			client->ws = ws;
			this->nextId++;
		},

		.message = [this](auto *ws, std::string_view message, uWS::OpCode opCode) {
			Client* client = ws->getUserData();

			if (opCode != uWS::OpCode::BINARY) {
				throw std::runtime_error{"Requires binary message"};
			}
			
			
			size_t size = message.size();
			static constexpr size_t ALIGN = 8; // 64-bit = 8 bytes

			uint8_t* const srcPtr = (uint8_t*) aligned_alloc(
				ALIGN,
				((size*1000 + ALIGN - 1) / ALIGN) * ALIGN
			);

			memcpy(srcPtr, message.data(), size);

			const uint8_t* ptr = srcPtr;

			
            try {
				uint8_t* res;
				#define get(name, fn) case ServerId::name:\
					res = this->fn(client, ptr);\
					break;
					

                switch ((ServerId)*ptr++) {
                    get(CONNECT, connect);
                    get(LISTEN, listen);
                    get(GAME_COMMANDS, runCommand);
                    get(UPDATE, getUpdates);
                    get(PANEL, getPanel);
                    get(ERROR, onerror);
                }
    
                #undef get
    
                if (res) {
                    uint32_t len = *(uint32_t*)res;
                    ws->send(std::string_view((char*)(res+4), len), uWS::OpCode::BINARY);
                    free(res);
                }
                
            } catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }

			free(srcPtr);
			
		},

		.close = [this](auto* ws, int code, std::string_view message) {
			Client* client = ws->getUserData();

			this->disconnect(client);

			this->clients.erase(client->id);

		}

	}).listen(port, [port](auto *listen_socket) {
		if (listen_socket) {
			printf("WebSocket Server listening on %d\n", port);
		}
	}).run();

	// Kill threads
	for (int i = 0; i < this->poolNum; i++)
		this->pools[i].stopThread();
	
	// Wait for threads
	for (int i = 0; i < this->poolNum; i++)
		this->pools[i].joinThread();
	
}