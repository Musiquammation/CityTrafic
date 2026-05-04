#include "Pool.hpp"

#include "Match.hpp"

#include <game/Game.hpp>
#include <cstring>

void runThread(Pool* pool) {
	using clock = std::chrono::steady_clock;
	const auto FRAME_TIME = std::chrono::microseconds(16667/8); // make faster
	// const auto FRAME_TIME = std::chrono::microseconds(16667);

	auto next = clock::now();

	while (pool->alive) {
		next += FRAME_TIME;

		// Run
		{
			std::lock_guard<std::mutex> structureLock{pool->structureMutex};
			for (auto& match : pool->matchs) {
				auto game = match.second->getGame<false>();
				game->frame();
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



