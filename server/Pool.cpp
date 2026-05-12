#include "Pool.hpp"

#include "Match.hpp"

#include <game/Game.hpp>
#include <cstring>

#include "game/serialization/serialize.hpp"
#include "game/utils/streams.hpp"



#ifndef TIME_MODE
#define TIME_MODE -1
#endif

#include "SaveGamePathFolder.hpp"

static void saveGame(hash_t hash, const Game& game) {
	return; // Cancel saving

	std::string path{SAVEGAME_FOLDERPATH};

	// Get path
	{
		char hexaHash[17];
		hash_toHexa(hash, hexaHash);
		hexaHash[16] = '\0';


		// Open file
		path += hexaHash;
	}

	printf("Saving %s\n", path.c_str());
	WriteStream stream{path};
	serialize::save(game, stream);
	printf("Saved  %s\n", path.c_str());
}

void runThread(Pool* pool) {
	using clock = std::chrono::steady_clock;

	#if TIME_MODE == 0 // fast
	const auto FRAME_TIME = std::chrono::microseconds(16667/4);
	#elif TIME_MODE == 1 // slow
	const auto FRAME_TIME = std::chrono::microseconds(16667*16);
	#else // basic
	const auto FRAME_TIME = std::chrono::microseconds(16667);
	#endif

	auto next = clock::now();

	while (pool->alive) {
		next += FRAME_TIME;

		// Run
		{
			std::lock_guard<std::mutex> structureLock{pool->structureMutex};
			for (auto [key, match] : pool->matchs) {
				auto game = match->getGame<false>();

				// Play game
				game->frame();

				// Check autosave
				if (match->checkAutoSave()) {
					saveGame(key, *game);
				}
			}
		}

		std::this_thread::sleep_until(next);
	}
}


void Pool::launchThread() {
	this->alive = true;
	this->thread = std::thread(runThread, this);
}

void Pool::stopThread() {
	this->alive = false;
}

void Pool::joinThread() {
	if (this->thread.joinable()) {
		this->thread.join();
	}
}

Match* Pool::createMatch(hash_t hash) {
	auto match = new Match{this, hash};
	printf("hash %ld\n", hash);
	
	std::lock_guard<std::mutex> structureLock{this->structureMutex};

	this->matchs[hash] = match;

	return match;
}

void Pool::deleteMatch(hash_t hash) {
	std::lock_guard<std::mutex> structureLock{this->structureMutex};

	auto it = this->matchs.find(hash);
	if (it != this->matchs.end()) {
		auto value = it->second;
		matchs.erase(it);
		delete value;
	}
}

Match* Pool::getMatch(hash_t hash) {
	auto it = this->matchs.find(hash);
	if (it != this->matchs.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}


void Pool::acquireGame(hash_t hash, int id) {
	std::unique_lock<std::mutex> lock(this->mutex);
    
	// Wait the other thread
	int other_thread = (Pool::THREAD_NUM-1) - id; 
	cv.wait(lock, [this, other_thread, hash] {
		return this->currents[other_thread] != hash;
	});
	
	// Take index
	this->currents[id] = hash;
}

void Pool::releaseGame(int id) {
	{
		std::lock_guard<std::mutex> lock{this->mutex};
		this->currents[id] = 0; // Remove ownership
	}
	
	cv.notify_one();
}













Pool::~Pool() {
	// Free objects in matchs
	for (auto& pair : this->matchs) {
		delete pair.second;
	}
}



