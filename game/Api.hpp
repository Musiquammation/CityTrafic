#pragma once

#include <vector>
#include <map>

#include "Game.hpp"

enum class ApiThreadState { ALIVE, FINISHING, DEAD };


enum class ApiTakeCode {
	FREE_BUFFER,

	MAKE_MAP,

	COPY_CARS,

	COPY_COORDS,

	MAKE_MAP_EDITS,
	MAKE_MAP_EDITS_ALL,

	TAKE_MAP_PTR,

	PUSH_LAYER,
	POP_LAYER,

	PLACE_SINGLE_ROAD,

	APPLY_EDITS,

	GAME_COMMAND,

	MAKE_ENTITIES,
	READ_ENTITIES
};


struct ApiGame {
	Game game;
};




class Api {
	int nextId;
	int indexSpacing;
	
	std::map<int, ApiGame> games;
	void* buffer = nullptr;

public:
	Api(int indexStart, int indexSpacing);
	~Api();

	int createSession();
	void deleteSession(int id);
	void* take(int id, int datacode, void* args);
	void runFrames();
};


extern "C" {
	Api* Api_createApi(int indexStart, int indexSpacing);
	void Api_deleteApi(Api* api);
	int Api_createSession(Api* api);
	void Api_deleteSession(Api* api, int id);
	void* Api_take(Api* api, int id, int datacode, void* args);
	void Api_runFrames(Api* api);
}