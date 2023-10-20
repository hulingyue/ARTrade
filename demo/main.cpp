#include <iostream>

#include <spdlog/spdlog.h>
#include "ws_client.h"

using namespace core::WebSocket;

void on_open(core::WebSocket::Client::Client& client) {
    spdlog::info("hahha");
}

int main() {
    core::WebSocket::Client::Client client;
    client.on_open = std::function<void(core::WebSocket::Client::Client&)>(on_open);
    client.connect("wss://stream.binance.com:9443/ws/btcusdt@trade");

    while (true) {

    }
    return 0;
}