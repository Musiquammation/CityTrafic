#pragma once

#include <mutex>
#include <vector>
#include <map>
#include <atomic>
#include <optional>
#include <mutex>

#include "Game.hpp"

enum class ApiThreadState { ALIVE, FINISHING, DEAD };


enum class ApiTakeCode {
	MAKE_MAP,

	COPY_CARS,
	FREE_CARS,

	COPY_COORDS,
	FREE_COORDS,

	TAKE_MAP_EDITS,
	TAKE_MAP_EDITS_ALL,
	RLSE_MAP_EDITS,

	TAKE_MAP_PTR,
	RLSE_MAP_PTR,

	PLACE_ROAD
};


struct ApiGame {
	Game game;
};




class Api {
public:
	Api(int indexStart, int indexSpacing);
	~Api();

	int createSession();
	void deleteSession(int id);
	void* take(int id, int datacode, void* args);
	void runFrames();

private:
	int nextId;
	int indexSpacing;
	
	std::shared_mutex mutex;
	std::map<int, ApiGame> games;
	std::atomic<ApiThreadState> state{ApiThreadState::ALIVE};
	void* buffer;
	std::optional<std::unique_lock<std::shared_mutex>> bffLock;

};


extern "C" {
	Api* Api_createApi(int indexStart, int indexSpacing);
	void Api_deleteApi(Api* api);
	int Api_createSession(Api* api);
	void Api_deleteSession(Api* api, int id);
	void* Api_take(Api* api, int id, int datacode, void* args);
	void Api_runFrames(Api* api);
}