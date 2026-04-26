#include "Client.hpp"

#include <string_view>
#include <uWebSockets/src/WebSocket.h>

void Client::send(void* data, int size) {
    this->ws->send(
        std::string_view((char*)data, size),
        uWS::OpCode::BINARY
    );
}