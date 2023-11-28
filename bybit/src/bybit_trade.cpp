#include <cassert>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include "bybit_trade.h"
#include <core/ws_client.h>
#include <core/http/util.hpp>
#include <core/http/client.h>
#include <core/util.h>
#include <core/time.hpp>

#include "format.hpp"


#define LOGHEAD "[BybitTrade::" + std::string(__func__) + "]"

namespace {
struct Self {
    std::atomic<bool> is_ready;
    core::websocket::client::WebSocketClient *websocket_client = nullptr;
    core::http::client::HttpClient http_client;

    std::string api_key;
    std::string api_secret;

    std::string category;

    unsigned long long reqid = 0;

    ~Self() {
        if (websocket_client) { delete websocket_client; }
    }
};

std::string generate_signature(uint64_t expires, std::string& api_secret) {
    std::string message = "GET/realtime" + std::to_string(expires);
    unsigned char* digest = HMAC(EVP_sha256(), api_secret.c_str(), api_secret.length(), reinterpret_cast<const unsigned char*>(message.c_str()), message.length(), nullptr, nullptr);
    char mdString[SHA256_DIGEST_LENGTH*2+1];
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    return std::string(mdString);
}

void update_headers(Self *self) {
    uint64_t timestamp = core::time::Time::now_millisecond();
    httplib::Headers headers = {
        {"X-BAPI-SIGN", generate_signature(timestamp, self->api_secret)},
        {"X-BAPI-API-KEY", self->api_key},
        {"X-BAPI-TIMESTAMP", std::to_string(timestamp)}
    };
    self->http_client.update_header(headers);
}

}


BybitTrade::BybitTrade() : self ( *new Self {} ) {
    self.is_ready.store(false);
}

BybitTrade::~BybitTrade() {
    delete &self;
}

void BybitTrade::init() {

    auto config = core::config::Config::get();
    self.api_key = config.value("api_key", "");
    self.api_secret = config.value("api_secret", "");
    assert(self.api_key.length() > 0 && self.api_secret.length() > 0);

    if (self.websocket_client) { delete self.websocket_client; }
    self.websocket_client = new core::websocket::client::WebSocketClient;
    self.websocket_client->on_open = std::function<void()>([this]() {
        on_open();
    });
    self.websocket_client->on_close = std::function<void()>([this]() {
        on_close();
    });
    self.websocket_client->on_message = std::function<void(std::string const &msg)>([this](std::string const &msg) {
        on_message(msg);
    });

    if (!config.contains("is_test") || !config["is_test"].is_boolean()) {
        spdlog::error("{} config error! cannot find 'is_test' or it's not a variable of bool");
        exit(-1);
    }

    if (!config.contains("is_spot") || !config["is_spot"].is_boolean()) {
        spdlog::error("{} config error! cannot find 'is_spot' or it's not a variable of bool");
        exit(-1);
    }

    bool is_test = config.value("is_test", true);
    std::string websocket_url = is_test ? config.value("trade_test_url", "") : config.value("trade_url", "");
    std::string restful_url = is_test ? config.value("trade_test_restful_url", "") : config.value("trade_restful_url", "");

    bool is_spot = config.value("is_spot", true);
    self.category = is_spot ? "spot" : "linear";

    spdlog::info("{} is_test: {} is_spot: {} websocket_url: {} restful_url: {}", LOGHEAD, is_test, is_spot, websocket_url, restful_url);
    // restful
    self.http_client.set_uri(restful_url);
    httplib::Headers headers = {
        {"Content-Type", "application/json"},
        {"X-BAPI-SIGN", ""},
        {"X-BAPI-API-KEY", ""},
        {"X-BAPI-TIMESTAMP", "0"},
        {"X-BAPI-RECV-WINDOW", "5000"}
    };
    self.http_client.set_header(headers);

    // websocket
    self.websocket_client->connect(websocket_url);
}

bool BybitTrade::is_ready() {
    return self.is_ready.load();
}

TradeOperateResult BybitTrade::order(core::datas::OrderObj const &order) {
    std::string path = "/v5/order/create";

    httplib::Params params;
    params.emplace("category", self.category);
    params.emplace("symbol", std::string(order.symbol));
    params.emplace("side", side_to_bybit(order.side));
    params.emplace("orderType", type_to_bybit(order.type));
    params.emplace("qty", std::to_string(order.quantity));
    params.emplace("price", std::to_string(order.price));
    params.emplace("timeInForce", tif_to_bybit(order.tif));

    update_headers(&self);
    httplib::Result res = self.http_client.post(path, params);

    TradeOperateResult result {
        .code = -1,
        .msg = ""
    };

    if (res) {
        result.code = res->status;
        result.msg = res->body;

        if (res->status == 200) {
            result.code = 0;
            spdlog::info("{} Request success, code: {} msg: {}", LOGHEAD, result.code, result.msg);
        } else {
            spdlog::error("{} Request failed, code: {} msg: {}", LOGHEAD, result.code, result.msg);
        }
    } else {
        httplib::Error err = res.error();
        result.code = static_cast<int>(err) * -1;
        result.msg = core::http::util::err_string(err);
        spdlog::error("{} Request failed, code: {} msg: {}", LOGHEAD, result.code, result.msg);
    }

    return result;
}

TradeOperateResult BybitTrade::cancel(core::datas::CancelObj const &order) {

}

void BybitTrade::interval_1s() {

}

void BybitTrade::on_open() {
    // self.is_ready.store(true);
    nlohmann::json json_obj;
    json_obj["req_id"] = self.reqid;
    json_obj["op"] = "auth";

    auto exipres = core::time::Time::now_millisecond() + 1000;
    json_obj["args"] = {
        self.api_key,
        exipres,
         generate_signature(exipres, self.api_secret)
    };

    std::string json_str = json_obj.dump();
    auto code = self.websocket_client->send(json_str);
    if (code == 0) {
        self.reqid = (self.reqid + 1) % ULLONG_MAX;
    }
}

void BybitTrade::on_close() {
    self.is_ready.store(false);
}

void BybitTrade::on_message(std::string const &msg) {
    nlohmann::json message = nlohmann::json::parse(msg);

    std::string op = message.value("op", "");
    if (op == "auth") {
        bool status = message.value("success", false);
        self.is_ready.store(status);

        if (status) {
            spdlog::info("{} op: {} status: {} msg: {}", LOGHEAD, op, status, message.value("ret_msg", ""));
        } else {
            spdlog::error("{} op: {} status: {} msg: {}", LOGHEAD, op, status, message.value("ret_msg", ""));
        }
    }
}

void BybitTrade::ping() {
    std::string json_obj = R"({"req_id": "0", "op": "ping"})";
    self.websocket_client->send(json_obj);
}

#undef LOGHEAD