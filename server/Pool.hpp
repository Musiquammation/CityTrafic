#pragma once

#include "declarations.hpp"
#include <game/hash.hpp>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <atomic>
#include <unordered_map>

class Pool {
	static constexpr int THREAD_NUM = 2;

	std::thread thread;
	std::mutex structureMutex;
	std::mutex mutex;
	std::condition_variable cv;

	std::atomic_bool alive = false;
	std::array<hash_t, 2> currents = {0, 0};
	std::unordered_map<hash_t, Match*> matchs;


	friend void runThread(Pool* pool);


public:
	static constexpr int POOL_ID = 0;
	static constexpr int SERVER_ID = 1;

	void launchThread();
	void stopThread();
	void joinThread();
	Match* createMatch(hash_t hash);
	void deleteMatch(hash_t hash);
	Match* getMatch(hash_t hash);

	void acquireGame(hash_t hash, int id);
	void releaseGame(int id);
	
	~Pool();
};