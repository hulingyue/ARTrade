#include "bybit_market.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <regex>


#define LOGHEAD "[BybitMarket::" + std::string(__func__) + "]"
using namespace core::WebSocket::Client;

namespace {
struct Self {
    std::atomic<bool> is_ready = false;
    Client *client = nullptr;
    int interval = 0;

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

static MarketOperateResult market_evene(Self &self, std::string op, const std::vector<std::string> symbols) {
    nlohmann::json json_obj;
    json_obj["req_id"] = op;
    json_obj["op"] = op;
    json_obj["args"] = {};
    for (auto symbol: symbols) {
        json_obj["args"].push_back(symbol);
    }
    std::string json_str = json_obj.dump();

    auto code = self.client->send(json_str);

    return {
        .code = code,
        .msg = ""
    };
}

MarketOperateResult BybitMarket::subscribe(const std::vector<std::string> symbols) {
    return market_evene(self, "subscribe", std::move(symbols));
}

MarketOperateResult BybitMarket::unsubscribe(const std::vector<std::string> symbols) {
    return market_evene(self, "unsubscribe", std::move(symbols));
}

void BybitMarket::interval_1s() {
    self.interval += 1;
    if (self.interval % 30 == 0) {
        ping();
    }
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
    nlohmann::json message = nlohmann::json::parse(msg);

    std::string topic = message.value("topic", "");
    
    std::regex pattern("([\\w]+)(\\.([\\w\\d]+))?\\.([\\w\\d]+)");
    std::smatch match;

    if (!std::regex_search(topic, match, pattern)) {
        spdlog::info("{} msg: {}", LOGHEAD, msg);
        return;
    }
    
    std::string mode = match[1];
    std::string frequence = match[3];
    std::string symbols = match[4];

    if (mode == "orderbook") {  //  orderbook.{depth}.{symbol} e.g., orderbook.1.BTCUSDT

    } else if (mode == "publicTrade") {  //  publicTrade.{symbol} 注意: 期權使用baseCoin, e.g., publicTrade.BTC

    } else if (mode == "tickers") {  //  tickers.{symbol}
        spdlog::info("{} tickers: {}", LOGHEAD, msg);

    } else if (mode == "kline") {  //  kline.{interval}.{symbol} e.g., kline.30.BTCUSDT

    } else if (mode == "liquidation") {  //  liquidation.{symbol} e.g., liquidation.BTCUSDT

    } else if (mode == "kline_lt") {  //  kline_lt.{interval}.{symbol} e.g., kline_lt.30.BTC3SUSDT

    } else if (mode == "tickers_lt") {  //  tickers_lt.{symbol} e.g.,tickers_lt.BTC3SUSDT

    } else if (mode == "lt") {  //  lt.{symbol} e.g.,lt.BTC3SUSDT

    } else {
        spdlog::info("{} msg: {}", LOGHEAD, msg);
    }

}

void BybitMarket::ping() {
    std::string json_obj = R"({"req_id": "0", "op": "ping"})";
    self.client->send(json_obj);
}

#undef LOGHEAD