#include <iostream>

#include <spdlog/spdlog.h>
#include "ws_client.h"


int main() {
    core::WebSocket::Client client;
    client.connect("wss://stream.binance.com:9443/ws/btcusdt@trade");

    while (true) {

    }
    return 0;
}