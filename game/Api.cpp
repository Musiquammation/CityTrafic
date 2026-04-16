#include "Api.hpp"
#include "Car.hpp"

#include <stdint.h>
#include <string.h>
#include <bit>

#define intArg(i) ((uint32_t*)args)[i]

Api::Api(int indexStart, int indexSpacing) :
	nextId(indexStart),
	indexSpacing(indexSpacing) {}

Api::~Api() {
}



int Api::createSession() {
	int id = this->nextId;
	this->nextId += this->indexSpacing;
	
	std::unique_lock<std::shared_mutex> lock{this->mutex};
	this->games.try_emplace(id);
	return id;
}

void Api::deleteSession(int id) {
	std::unique_lock<std::shared_mutex> lock{this->mutex};
	this->games.erase(id);
}

void* Api::take(int id, int datacode, void* args) {
	std::shared_lock<std::shared_mutex> lock{this->mutex};

	switch ((ApiTakeCode)datacode) {
	case ApiTakeCode::FREE_BUFFER:
		if (this->buffer)
			free(this->buffer);
		
		return nullptr;


	case ApiTakeCode::MAKE_MAP:
	{
		ApiGame& s = this->games[id];
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

		this->buffer = array;
		return array;
	}


	case ApiTakeCode::COPY_CARS:
	{
		ApiGame& s = this->games[id];
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

		this->buffer = buffer;
		return buffer;
	}


	case ApiTakeCode::COPY_COORDS:
	{
		ApiGame& s = this->games[id];
		int32_t* array = (int32_t*)malloc(sizeof(int32_t) * 4);
		MapSize size = s.game.map.getMapSize();
		array[0] = size.x;
		array[1] = size.y;
		array[2] = size.width;
		array[3] = size.height;

		this->buffer = array;
		return this->buffer;
	}

	case ApiTakeCode::MAKE_MAP_EDITS:
	{
		ApiGame& s = this->games[id];
		auto lock = s.game.mutexPool.lockRead(MutexLabel::MAP);

		int x0 = intArg(0);
		int y0 = intArg(1);
		int w  = intArg(2);
		int h  = intArg(3);
		int layer = intArg(4);

		uint32_t* buffer = s.game.map.collectEditedCells(
			x0, y0, w, h, layer);

		this->buffer = buffer;
		return buffer;
	}

	case ApiTakeCode::MAKE_MAP_EDITS_ALL:
	{
		ApiGame& s = this->games[id];
		int layer = intArg(0);
		auto lock = s.game.mutexPool.lockRead(MutexLabel::MAP);
		uint32_t* buffer = s.game.map.collectEditedCells(layer);
		this->buffer = buffer;
		return buffer;
	}


	case ApiTakeCode::TAKE_MAP_PTR:
	{
		ApiGame& s = this->games[id];
		this->bffLock = s.game.mutexPool.lockWrite(MutexLabel::MAP);
		return s.game.map.cells;
	}

	case ApiTakeCode::RLSE_MAP_PTR:
	{
		this->bffLock.reset();
		return nullptr;
	}

	case ApiTakeCode::PLACE_SINGLE_ROAD:
	{
		ApiGame& s = this->games[id];
		auto lock = s.game.mutexPool.lockWrite(MutexLabel::MAP);
		Cell* cell = s.game.getEditCell(intArg(0), intArg(1));
		cell->setType(CellType::ROAD);
		return nullptr;
	}

	case ApiTakeCode::PUSH_LAYER:
	{
		ApiGame& s = this->games[id];
		int* ret = (int*)malloc(sizeof(int));
		*ret = s.game.map.addEditedCellsLayer();
		this->buffer = ret;
		return ret;
	}

	case ApiTakeCode::POP_LAYER:
	{
		ApiGame& s = this->games[id];
		s.game.map.removeEditedCellsLayer(intArg(0));
		return nullptr;
	}

	case ApiTakeCode::APPLY_EDITS:
	{
		ApiGame& s = this->games[id];
		auto lock = s.game.mutexPool.lockRead(MutexLabel::MAP);
		s.game.map.applyEdits((uint32_t*)args);
		return nullptr;
	}


	default:
		return nullptr;
	}
}

void Api::runFrames() {
	std::shared_lock<std::shared_mutex> lock{this->mutex};
	
	for (auto& [_, i]: this->games) {
		i.game.frame();
	}
}




Api* Api_createApi(int indexStart, int indexSpacing) {
	Api* api = new Api{indexStart, indexSpacing};
	return api;
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

void Api_runFrames(Api* api) {
	api->runFrames();
}