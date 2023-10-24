#include "bybit_market.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>


#define LOGHEAD "[BybitMarket::" + std::string(__func__) + "]"
using namespace core::WebSocket::Client;

namespace {
struct Self {
    std::atomic<bool> is_ready = false;
    Client *client = nullptr;

    ~Self() {
        if (client) delete client;
    }
};
}

BybitMarket::BybitMarket() : self { *new Self{} } {
    self.client = new Client();
    self.client->on_open = std::function<void()>([this]() {
        on_open();
    });
    self.client->on_close = std::function<void()>([this]() {
        on_close();
    });
    self.client->on_message = std::function<void(std::string const &msg)>([this](std::string const &msg) {
        on_message(msg);
    });

    self.client->connect("wss://stream-testnet.bybit.com/v5/public/spot");
}

BybitMarket::~BybitMarket() {
    if (&self) delete &self;
}

bool BybitMarket::is_ready() {
    return self.is_ready.load();
}

MarketOperateResult BybitMarket::subscribe() {
    nlohmann::json json_obj;
    json_obj["req_id"] = "test";
    json_obj["op"] = "subscribe";
    json_obj["args"] = {};
    json_obj["args"].push_back("tickers.BTCUSDT");
    std::string json_str = json_obj.dump();

    std::cout << json_str << std::endl;

    self.client->send(json_str);

    return {
        .code = 0,
        .msg = ""
    };
}

MarketOperateResult BybitMarket::unsubscribe() {

    return {
        .code = 0,
        .msg = ""
    };
}


/***********************/
/**  WebSocket Event  **/
/***********************/ 

void BybitMarket::on_open() {
    spdlog::info("{}", LOGHEAD);
    self.is_ready.store(true);
}

void BybitMarket::on_close() {
    spdlog::info("{}", LOGHEAD);
    self.is_ready.store(false);
}

void BybitMarket::on_message(const std::string &msg) {
    spdlog::info("{} msg: {}", LOGHEAD, msg);
}

#undef LOGHEAD