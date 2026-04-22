#pragma once

#include "hash.hpp"
#include "Match.hpp"

#include <thread>
#include <shared_mutex>
#include <atomic>
#include <unordered_map>

class Pool {
    std::thread thread;
    std::shared_mutex mutex;
    std::atomic_bool alive = false;
    std::unordered_map<hash_t, Match*> matchs;

public:
    void launchThread();
    void stopThread();
    void joinThread();
    Match* createMatch(hash_t hash);
    void deleteMatch(hash_t hash);
    Match* getMatch(hash_t hash);

    ~Pool();
};