#include <iostream>
#include <ctime>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "ws_client.h"

using namespace core::WebSocket;
using namespace std::chrono_literals;

class Demo {
public:
    Demo() {}

    ~Demo() {
        if (client) {
            client->stop();
        }
    }

    void run() {
        client = new core::WebSocket::Client::Client();
        client->on_open = std::function<void()>([this]() {
            on_open();
        });
        client->connect("wss://stream.binance.com:9443/ws");

        std::this_thread::sleep_for(2000ms);
        nlohmann::json json_obj;
        json_obj["method"] = "SUBSCRIBE";
        json_obj["params"] = {"btcusdt@depth"};
        json_obj["id"] = 1;
        std::string json_str = json_obj.dump();

        std::cout << json_str << std::endl;

        client->send(json_str);
    }

    void on_open() {
        spdlog::info("hahha");
    }

private:
    core::WebSocket::Client::Client *client;
};

int main() {
    Demo d;
    d.run();
    while (true) {

    }
    return 0;
}