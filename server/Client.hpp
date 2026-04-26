#pragma once

#include "declarations.hpp"
#include "clientId_t.hpp"

#include <set>

namespace uWS {
	template<bool A, bool B, typename C>
	struct WebSocket;
};

struct Client {
	int viewX = 0;
	int viewY = 0;
	int viewW = 0;
	int viewH = 0;
	clientId_t id = 0;
	Match* match;

	std::set<uint64_t> visitedRegions;
	uWS::WebSocket<false, true, Client>* ws = nullptr;

	void send(void* data, int size);
};
