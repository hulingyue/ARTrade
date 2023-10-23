#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "binance_market.h"
#include "ws_client.h"


using namespace core::WebSocket::Client;

namespace {
struct Self {
    Client *client = nullptr;

    ~Self() {
        if (client) delete client;
    }
};
}

BinanceMarket::BinanceMarket() : self { *new Self{} } {
    self.client = new Client();
    self.client->on_open = std::function<void()>([this]() {
        on_open();
    });
    self.client->connect("wss://stream.binance.com:9443/ws");
}

BinanceMarket::~BinanceMarket() {
    if (&self) delete &self;
}

bool BinanceMarket::is_ready() {
    return true;
}

MarketOperateResult BinanceMarket::subscribe() {
    nlohmann::json json_obj;
    json_obj["method"] = "SUBSCRIBE";
    json_obj["params"] = {"btcusdt@depth"};
    json_obj["id"] = 1;
    std::string json_str = json_obj.dump();

    std::cout << json_str << std::endl;

    self.client->send(json_str);

    return {
        .code = 0,
        .msg = ""
    };
}

MarketOperateResult BinanceMarket::unsubscribe() {
    return {
        .code = 0,
        .msg = ""
    };
}

void BinanceMarket::on_open() {

}