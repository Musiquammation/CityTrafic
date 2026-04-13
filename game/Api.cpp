#include "Api.hpp"

#include <string.h>

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

void* Api::take(int id, int datacode, void* args) {
	ApiThread& thread = threads[id % threadnum];
	ApiGame& s = thread.games[threadnum];

	switch ((ApiTakeCode)datacode) {
	case ApiTakeCode::TAKE_MAP_CPY:
	{
		int x0 = ((uint32_t*)args)[0];
		int y0 = ((uint32_t*)args)[1];
		int w  = ((uint32_t*)args)[2];
		int h  = ((uint32_t*)args)[3];


		auto lock = s.game.mutexPool.lockRead(MutexLabel::MAP);
		Cell* array = (Cell*)malloc(sizeof(Cell) * w*h);
		const Cell* cells = s.game.map.cells;
		int gridWidth = s.game.map.width;

		int dx = x0 - s.game.map.x;
		int dy = y0 - s.game.map.y;


		/// TODO: test this portion
		for (int y = 0; y < h; y++) {
			int pos = (dy+y) * gridWidth + dx;

			memcpy(
				&array[y*w],
				&array[pos],
				w*sizeof(Cell)
			);
		}

		thread.buffer = array;
		return array;
	}

	case ApiTakeCode::RLSE_MAP_CPY:
	{
		free(thread.buffer);
		return nullptr;
	}

	case ApiTakeCode::TAKE_COORDS:
	{
		int32_t* array = (int32_t*)malloc(sizeof(int32_t) * 4);
		MapSize size = s.game.map.getMapSize();
		array[0] = size.x;
		array[1] = size.y;
		array[2] = size.width;
		array[3] = size.height;

		thread.buffer = array;
		return thread.buffer;
	}

	case ApiTakeCode::RSLE_COORDS:
	{
		free(thread.buffer);
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

void* Api_take(Api* api, int id, int datacode, void* args) {
	return api->take(id, datacode, args);
}
