#include <iostream>
#include <ctime>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "ws_client.h"

using namespace core::WebSocket;
using namespace std::chrono_literals;

void on_open(core::WebSocket::Client::Client& client) {
    spdlog::info("hahha");
}

int main() {
    core::WebSocket::Client::Client client;
    client.on_open = std::function<void(core::WebSocket::Client::Client&)>(on_open);
    client.connect("wss://stream.binance.com:9443/ws");
    
    std::this_thread::sleep_for(10000ms);
    nlohmann::json json_obj;
    json_obj["method"] = "SUBSCRIBE";
    json_obj["params"] = {"btcusdt@depth"};
    json_obj["id"] = 1;
    std::string json_str = json_obj.dump();

    std::cout << json_str << std::endl;

    client.send(json_str);

    while (true) {

    }
    return 0;
}