#include "bybit_market.h"
#include <core/util.h>
#include <core/http/util.hpp>
#include <core/http/client.h>
#include <core/time.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <regex>


#define LOGHEAD "[BybitMarket::" + std::string(__func__) + "]"
using namespace core::websocket::client;

namespace {
struct Self {
    core::http::client::HttpClient http_client;
    std::atomic<bool> is_ready;
    WebSocketClient *client = nullptr;
    int interval = 0;

    // market
    std::unordered_map<std::string_view, double> MarketBBOPrices;

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
    // tudo: symbol转换
    return market_evene(self, "subscribe", std::move(symbols));
}

MarketOperateResult BybitMarket::unsubscribe(const std::vector<std::string> symbols) {
    // tudo: symbol转换
    return market_evene(self, "unsubscribe", std::move(symbols));
}

void BybitMarket::interval_1s() {
    self.interval += 1;
    if (self.interval % 20 == 0) {
        ping();
    }
}

void BybitMarket::instruments() {
    auto config = core::config::Config::get();
    bool is_test = config.value("is_test", true);
    std::string base_url = is_test ? config.value("trade_test_restful_url", "") : config.value("trade_restful_url", "");
    std::string category = config.value("is_spot", true) ? "spot" : "linear";
    self.http_client.set_uri(base_url);
    
    std::string url = "/v5/market/instruments-info?category=" + category;// + "&limit=1000";

    httplib::Headers headers = {{"Content-Type", "application/json"}};
    self.http_client.set_header(headers);

    httplib::Result res = self.http_client.get(url);

    if (!res) {
        spdlog::error("{} url: {}", LOGHEAD, url);
        return;
    }

    if (res->status != 200) {
        spdlog::error("{} url: {} state: {}", LOGHEAD, url, res->status);
        return;
    }

    nlohmann::json message = nlohmann::json::parse(res->body);
    if (message["retCode"] != 0 || message["retMsg"] != "OK") {
        spdlog::error("{} url: {} state: {} retCode: {} retMsg: {}", LOGHEAD, url, res->status, message["retCode"], message["retMsg"]);
        return;
    }

    message = message["result"]["list"];

    for (nlohmann::json item: message) {
        if (item["status"] != "Trading") { continue; }
        core::datas::Instruments* instrument = new core::datas::Instruments {
            .type = core::datas::InstrumentType::Type::Future,
            .symbol = item["symbol"],
            .log_size = 1,
            .price_scale = pow(10, -1 * std::stoi(std::string(item["priceScale"]))),
            .min_leverage = std::stod(std::string(item["leverageFilter"]["minLeverage"])),
            .max_leverage = std::stod(std::string(item["leverageFilter"]["maxLeverage"])),
            .leverage_scale = std::stod(std::string(item["leverageFilter"]["leverageStep"])),
            .min_quantity = std::stod(std::string(item["lotSizeFilter"]["minOrderQty"])),
            .max_quantity = std::stod(std::string(item["lotSizeFilter"]["maxOrderQty"])),
            .quantity_scale = std::stod(std::string(item["lotSizeFilter"]["qtyStep"])),
            .expire = core::time::Time(2099, 12, 31, 23, 59, 59).to_string()
        };
        modules()->add_instrument(instrument);
    }

    return;
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
        obj.time = message["ts"].get<uint64_t>();
        if (message["data"].is_null() || message["data"]["lastPrice"].is_null()) {
            // Bybit 首条数据一定是snapshot，所以理论上不必判断key是否存在
            obj.price = self.MarketBBOPrices[symbols];
        } else {
            obj.price = std::stod(message["data"]["lastPrice"].get<std::string>());
            self.MarketBBOPrices[symbols] = obj.price;
        }
        obj.quantity = 0;

        on_market(obj);
        return;

    } else if (mode == "kline") {  //  kline.{interval}.{symbol} e.g., kline.30.BTCUSDT
        // spdlog::info("{} tickers: {}", LOGHEAD, msg);

        core::datas::Market_kline obj;
        obj.market_type = core::datas::MarketType::Kline;
        std::strcpy(obj.symbol, symbols.c_str());
        std::strcpy(obj.exchange, "Bybit");
        obj.time = message["ts"].get<uint64_t>();

        // only one from a practical perspective
        auto data = message["data"][0];
        obj.confirm = data["confirm"];

        std::string interval = data["interval"];
        if (std::isdigit(interval[0])) {
            int i_interval = std::stod(interval);
            if (i_interval < 60) {
                interval = interval + "min";
            } else {
                i_interval = i_interval / 60;
                interval = std::to_string(i_interval) + "h";
            }
        }
        std::strcpy(obj.interval, interval.c_str());

        obj.high = std::stod(data["high"].get<std::string>());
        obj.low = std::stod(data["low"].get<std::string>());
        obj.open = std::stod(data["open"].get<std::string>());
        obj.close = std::stod(data["close"].get<std::string>());
        obj.start = data["start"];
        obj.end = data["end"];

        on_market(obj);
        return;
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