#include "Api.hpp"


Api::Api(int threadnum) : threadnum(threadnum), threads(threadnum) {}

Api::~Api() {
	for (auto& t : threads) t.state = ApiThreadState::DEAD;
	for (auto& t : threads) if (t.thread.joinable()) t.thread.join();
}

void Api::init() {
	// Create threads
	for (auto& t : threads) {
		t.thread = std::thread([&t](){
			using clock = std::chrono::steady_clock;
			const auto frameTime = std::chrono::milliseconds(16);
			while (t.state.load() == ApiThreadState::ALIVE) {
				auto start = clock::now();
				{
					std::lock_guard<std::mutex> lock(t.mutex);
					for (auto& g : t.games) g.second.game.frame();
				}

				auto elapsed = clock::now() - start;
				if (elapsed < frameTime) {
					std::this_thread::sleep_for(frameTime - elapsed);
				}
			}
		});
	}
}

int Api::createSession() {
	int id = nextId++;
	ApiThread& thread = threads[id % threadnum];
	std::lock_guard<std::mutex> lock(thread.mutex);
	thread.games.try_emplace(id);
	return id;
}

void Api::deleteSession(int id) {
	ApiThread& thread = threads[id % threadnum];
	std::lock_guard<std::mutex> lock(thread.mutex);
	thread.games.erase(id);
}

void* Api::take(int id, int datacode) {
	ApiGame& s = threads[id % threadnum].games[threadnum];

	switch ((ApiTakeCode)datacode) {
	case ApiTakeCode::TAKE_MAP:
	{
		s.locks[(size_t)ApiGameLockCode::MAP] =
			s.game.mutexPool.lockRead(MutexLabel::MAP);

		return s.game.map.cells;
	}

	case ApiTakeCode::RLSE_MAP:
	{
		s.locks[(size_t)ApiGameLockCode::MAP].reset(); // unlock
		return nullptr;
	}

	case ApiTakeCode::TAKE_COORDS:
	{
		this->buffer = malloc(sizeof(MapSize));
		*(MapSize*)this->buffer = s.game.map.getMapSize();
		return this->buffer;
	}

	case ApiTakeCode::RSLE_COORDS:
	{
		free(this->buffer);
		return nullptr;
	}


	default:
		return nullptr;
	}
}




Api* Api_createApi(int threadnum) {
	return new Api(threadnum);
}

void Api_deleteApi(Api* api) {
	delete api;
}

int Api_createSession(Api* api) {
	return api->createSession();
}

void Api_deleteSession(Api* api, int id) {
	api->deleteSession(id);
}

void* Api_take(Api* api, int id, int datacode) {
	return api->take(id, datacode);
}
