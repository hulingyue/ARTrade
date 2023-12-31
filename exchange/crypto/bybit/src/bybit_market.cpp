#include "bybit_market.h"
#include <core/util.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <regex>


#define LOGHEAD "[BybitMarket::" + std::string(__func__) + "]"
using namespace core::websocket::client;

namespace {
struct Self {
    std::atomic<bool> is_ready;
    WebSocketClient *client = nullptr;
    int interval = 0;

    // market
    std::unordered_map<char*, double> MarketBBOPrices;

    ~Self() {
        if (client) delete client;
    }
};
}

BybitMarket::BybitMarket() : self { *new Self{} } {
    self.is_ready.store(false);
}

BybitMarket::~BybitMarket() {
    if (&self) delete &self;
}

void BybitMarket::init() {
    if (self.client) { delete self.client; }
    self.client = new WebSocketClient();
    self.client->on_open = std::function<void()>([this]() {
        on_open();
    });
    self.client->on_close = std::function<void()>([this]() {
        on_close();
    });
    self.client->on_message = std::function<void(std::string const &msg)>([this](std::string const &msg) {
        on_message(msg);
    });

    auto config = core::config::Config::get();

    if (!config.contains("is_test") || !config["is_test"].is_boolean()) {
        spdlog::error("{} config error! cannot find 'is_test' or it's not a variable of bool");
        exit(-1);
    }

    if (!config.contains("is_spot") || !config["is_spot"].is_boolean()) {
        spdlog::error("{} config error! cannot find 'is_spot' or it's not a variable of bool");
        exit(-1);
    }

    bool is_test = config.value("is_test", true);
    bool is_spot = config.value("is_spot", true);

    std::string url;
    if (is_test) {
        url = is_spot ? config.value("market_test_spot_url", "") : config.value("market_spot_url", "");
    } else {
        url = is_spot ? config.value("market_test_future_url", "") : config.value("market_future_url", "");
    }

    spdlog::info("{} is_test: {} is_spot: {} url: {}", LOGHEAD, is_test, is_spot, url);
    self.client->connect(url);
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
    // delta: 如果delta數據中缺失一些字段，表明該字段自上次推送以來沒有發生變化。
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
        // spdlog::info("{} tickers: {}", LOGHEAD, msg);
        core::datas::Market_bbo obj;
        obj.market_type = core::datas::MarketType::Bbo;
        std::strcpy(obj.symbol, symbols.c_str());
        std::strcpy(obj.exchange, "Bybit");
        obj.time = message.value("ts", 0);
        if (message["data"].is_null() || message["data"]["lastPrice"].is_null()) {
            // Bybit 首条数据一定是snapshot，所以理论上不必判断key是否存在
            obj.price = self.MarketBBOPrices[obj.symbol];
        } else {
            obj.price = std::stod(message["data"]["lastPrice"].get<std::string>());
            self.MarketBBOPrices[obj.symbol] = obj.price;
        }
        obj.quantity = 0;

        on_market(obj);
        return;

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