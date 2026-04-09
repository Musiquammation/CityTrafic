#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <atomic>

#include "Game.hpp"

enum class ApiThreadState { ALIVE, FINISHING, DEAD };

struct ApiThread {
    std::thread thread;
    std::mutex mutex;
    std::map<int, Game> games;
    std::atomic<ApiThreadState> state{ApiThreadState::ALIVE};
};

class Api {
public:
    Api(int threadnum);
    ~Api();

    void init();
    int createSession();
    void deleteSession(int id);
    void* take(int id, int datacode);

private:
    int threadnum;
    int nextId{0};
    std::vector<ApiThread> threads;
};