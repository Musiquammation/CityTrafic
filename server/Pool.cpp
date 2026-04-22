#include "Pool.hpp"

#include <cstring>

void runThread(Pool* pool) {

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
	auto match = new Match{};
	
	std::shared_lock<std::shared_mutex> lock{this->mutex};

	this->matchs[hash] = match;

	return match;
}

void Pool::deleteMatch(hash_t hash) {
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

Pool::~Pool() {
	// Free objects in matchs
	for (auto& pair : this->matchs) {
		delete pair.second;
	}
}




