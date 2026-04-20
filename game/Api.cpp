#include "Api.hpp"

#include "Car.hpp"
#include "Character.hpp"
#include "runGameCommand.hpp"

#include <stdint.h>
#include <string.h>
#include <bit>

#include <stdio.h>

#define intArg(i) ((uint32_t*)args)[i]

void* Api::makeEntities(
	Game& game,
	int x, int y, int w, int h
) {
	float fx0 = (float)x;
	float fy0 = (float)y;
	float fx1 = (float)(x+w);
	float fy1 = (float)(y+h);


	uint32_t carsCount = 0;
	uint32_t charactersCount = 0;

	for (auto [_, car]: game.carHandler) {
		if (car->x >= x && car->x < x+w
			&& car->y >= y && car->y < y+h
		) {
			carsCount++;
		}
	}

	for (auto character: game.characterHandler.characters) {
		if (character->state == CharacterState::WALK
			&& character->x >= fx0 && character->x < fx1
			&& character->y >= fy0 && character->y < fy1
		) {
			charactersCount++;
		}
	}


	uint32_t fullSize = sizeof(uint32_t) *
		(carsCount*5 + charactersCount*2 + 3);

	uint32_t* const buffer = (uint32_t*)malloc(fullSize);

	uint32_t* ptr = buffer;
	
	*ptr++ = fullSize;

	// Send cars
	*ptr++ = carsCount;

	for (auto [_, car]: game.carHandler) {
		if (car->x >= x && car->x < x+w
			&& car->y >= y && car->y < y+h
		) {
			float speed = car->getSpeed();
			uint32_t flag = (uint32_t)car->state |
				((uint32_t)car->direction << 8);

			*ptr++ = car->x;
			*ptr++ = car->y;
			*ptr++ = *(uint32_t*)&car->step;
			*ptr++ = *(uint32_t*)&speed;
			*ptr++ = flag;
		}
	}

	// Send characters
	*ptr++ = charactersCount;
	for (auto character: game.characterHandler.characters) {
		if (character->state == CharacterState::WALK
			&& character->x >= fx0 && character->x < fx1
			&& character->y >= fy0 && character->y < fy1
		) {
			*ptr++ = *(uint32_t*)&character->x;
			*ptr++ = *(uint32_t*)&character->y;
		}
	}


	return buffer;
}


void Api::readEntities(Game& game, void* args) {
	uint32_t* ptr = (uint32_t*)args;

	game.map.resetCarMarks();
	game.carHandler.cars.clear();

	uint32_t carsCount = *ptr++;

	for (uint32_t i = 0; i < carsCount; i++) {
		int x = (int)(*ptr++);
		int y = (int)(*ptr++);
		float step = *(float*)(ptr++);
		float speed = *(float*)(ptr++);
		uint32_t flag = *ptr++;
		Direction direction = (Direction)(flag >> 8);
		CarState state = (CarState)(flag & 0xff);

		Car* car = game.spawnCar(x, y, direction);
		car->step = step;
		car->state = state;
		car->realSpeed = speed;
		car->publicSpeed = speed;
	}

	uint32_t charactersCount = *ptr++;
	for (uint32_t i = 0; i < charactersCount; i++) {
		float x = *(float*)(ptr++);
		float y = *(float*)(ptr++);

		Character* character = new Character{x, y, CharacterState::WALK};
		game.characterHandler.characters.push_back(character);
	}
}



Api::Api(int indexStart, int indexSpacing) :
	nextId(indexStart),
	indexSpacing(indexSpacing) {}

Api::~Api() {
}



int Api::createSession() {
	int id = this->nextId;
	this->nextId += this->indexSpacing;
	
	this->games.try_emplace(id);
	return id;
}

void Api::deleteSession(int id) {
	this->games.erase(id);
}

void* Api::take(int id, int datacode, void* args) {
	try {

		switch ((ApiTakeCode)datacode) {
		case ApiTakeCode::FREE_BUFFER:
			if (this->buffer) {
				free(this->buffer);
				this->buffer = nullptr;
			}
			
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
			uint32_t* buffer = s.game.map.collectEditedCells(layer);
			this->buffer = buffer;
			return buffer;
		}
	
	
		case ApiTakeCode::TAKE_MAP_PTR:
		{
			ApiGame& s = this->games[id];
			return s.game.map.cells;
		}
	
	
		case ApiTakeCode::PLACE_SINGLE_ROAD:
		{
			ApiGame& s = this->games[id];
			int x = (int)intArg(0);
			int y = (int)intArg(1);

			if (!s.game.checkBounds(x,y,1,1))
				return nullptr;
				
			Cell* cell = s.game.getEditCell(x, y);
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
			s.game.map.applyEdits((uint32_t*)args);
			return nullptr;
		}

		case ApiTakeCode::GAME_COMMAND:
		{
			ApiGame& s = this->games[id];

			return runGameCommand(s.game, args);
		}

		case ApiTakeCode::MAKE_ENTITIES:
		{
			ApiGame& s = this->games[id];
			void* buffer = Api::makeEntities(s.game,
				intArg(0), intArg(1), intArg(2), intArg(3));

			this->buffer = buffer;
			return buffer;
		}

		case ApiTakeCode::READ_ENTITIES:
		{
			ApiGame& s = this->games[id];
			Api::readEntities(s.game, args);
			return nullptr;
		}
	
	
		default:
			return nullptr;
		}
	} catch (std::exception& e) {
		printf("ApiError: %s\n", e.what());
	}

	return nullptr;
}

void Api::runFrames() {
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