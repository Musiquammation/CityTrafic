#include "Api.hpp"
#include "Car.hpp"

#include <stdint.h>
#include <string.h>
#include <bit>

#define intArg(i) ((uint32_t*)args)[i]

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
	case ApiTakeCode::MAKE_MAP:
	{
		int x0 = intArg(0);
		int y0 = intArg(1);
		int w  = intArg(2);
		int h  = intArg(3);


		Cell* array = (Cell*)malloc(sizeof(Cell) * w*h);
		const Cell* cells = s.game.map.cells;
		int gridWidth = s.game.map.width;

		int dx = x0 - s.game.map.x;
		int dy = y0 - s.game.map.y;


		auto lock = s.game.mutexPool.lockRead(MutexLabel::MAP);
		
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


	case ApiTakeCode::COPY_CARS:
	{
		auto lockStructure = s.game.mutexPool.lockRead(MutexLabel::CARS_STRUCTURE);

		uint32_t carSize = (uint32_t)s.game.carHandler.cars.size();
		/**
		 * 1st element for length
		 * 
		 * Structure:
		 * [+0]: x
		 * [+1]: y
		 * [+2]: step (float)
		 * [+3]: direction, state
		 */
		uint32_t* const buffer = (uint32_t*)malloc(sizeof(uint32_t) +
			(4*sizeof(uint32_t)) * carSize);


		auto lockPositions = s.game.mutexPool.lockRead(MutexLabel::CARS_POSITIONS);

		uint32_t* ptr = buffer;
		*ptr++ = (uint32_t)carSize;
		for (auto i: s.game.carHandler.cars) {
			Car* car = i.second;
			*ptr++ = *(uint32_t*)(&car->x);
			*ptr++ = *(uint32_t*)(&car->y);
			*ptr++ = *(uint32_t*)(&car->step);
			*ptr++ = (uint32_t(car->direction) & 0xff)
	   			| ((uint32_t(car->state) & 0xff) << 8);
		}

		thread.buffer = buffer;
		return buffer;
	}

	case ApiTakeCode::FREE_CARS:
	{
		free(thread.buffer);
		return nullptr;
	}

	case ApiTakeCode::COPY_COORDS:
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

	case ApiTakeCode::FREE_COORDS:
	{
		free(thread.buffer);
		return nullptr;
	}

	case ApiTakeCode::TAKE_MAP_EDITS:
	{
		auto lock = s.game.mutexPool.lockRead(MutexLabel::MAP);

		int x0 = intArg(0);
		int y0 = intArg(1);
		int w  = intArg(2);
		int h  = intArg(3);

		uint32_t* buffer = s.game.map.collectEditedCells(x0, y0, w, h);

		thread.buffer = buffer;
		return buffer;
	}

	case ApiTakeCode::TAKE_MAP_EDITS_ALL:
	{
		auto lock = s.game.mutexPool.lockRead(MutexLabel::MAP);
		uint32_t* buffer = s.game.map.collectEditedCells();
		thread.buffer = buffer;
		return buffer;
	}

	case ApiTakeCode::RLSE_MAP_EDITS:
	{
		free(thread.buffer);
		return nullptr;
	}

	case ApiTakeCode::TAKE_MAP_PTR:
	{
		thread.lock = s.game.mutexPool.lockWrite(MutexLabel::MAP);
		return s.game.map.cells;
	}

	case ApiTakeCode::RLSE_MAP_PTR:
	{
		thread.lock.reset();
	}

	case ApiTakeCode::PLACE_ROAD:
	{
		auto lock = s.game.mutexPool.lockWrite(MutexLabel::MAP);
		Cell* cell = s.game.getEditCell(intArg(0), intArg(1));
		cell->setType(CellType::ROAD);
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
