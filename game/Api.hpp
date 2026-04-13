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
	TAKE_MAP,
	RLSE_MAP,

	TAKE_COORDS,
	RSLE_COORDS
};

enum class ApiGameLockCode {
	MAP,

	COUNT
};

struct ApiGame {
	Game game;
	std::array<
		std::optional<std::shared_lock<std::shared_mutex>>,
		(size_t)ApiGameLockCode::COUNT
	> locks;
};

struct ApiThread {
	std::thread thread;
	std::mutex mutex;
	std::map<int, ApiGame> games;
	std::atomic<ApiThreadState> state{ApiThreadState::ALIVE};
};



class Api {
public:
	Api(int threadnum);
	~Api();

	void init();
	int createSession();
	void deleteSession(int id);
	void* take(int id, int datacode);

private:
	void* buffer;
	int threadnum;
	int nextId{0};
	std::vector<ApiThread> threads;
};


extern "C" {
	Api* Api_createApi(int threadnum);
	void Api_deleteApi(Api* api);
	int Api_createSession(Api* api);
	void Api_deleteSession(Api* api, int id);
	void* Api_take(Api* api, int id, int datacode);
}