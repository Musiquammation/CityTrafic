#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <atomic>
#include <optional>

#include "Game.hpp"

enum class ApiThreadState { ALIVE, FINISHING, DEAD };


enum class ApiTakeCode {
	TAKE_MAP_CPY,
	RLSE_MAP_CPY,

	TAKE_COORDS,
	RSLE_COORDS
};


struct ApiGame {
	Game game;
};

struct ApiThread {
	std::thread thread;
	std::mutex mutex;
	std::map<int, ApiGame> games;
	std::atomic<ApiThreadState> state{ApiThreadState::ALIVE};
	void* buffer;
};



class Api {
public:
	Api(int threadnum);
	~Api();

	void init();
	int createSession();
	void deleteSession(int id);
	void* take(int id, int datacode, void* args);

private:
	int threadnum;
	int nextId{0};
	std::vector<ApiThread> threads;
};


extern "C" {
	Api* Api_createApi(int threadnum);
	void Api_deleteApi(Api* api);
	int Api_createSession(Api* api);
	void Api_deleteSession(Api* api, int id);
	void* Api_take(Api* api, int id, int datacode, void* args);
}