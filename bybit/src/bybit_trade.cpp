#include <cassert>
#include "bybit_trade.h"
#include <core/util.h>
#include <core/time.hpp>


#define LOGHEAD "[BybitTrade::" + std::string(__func__) + "]"
using namespace core::WebSocket::Client;

namespace {
struct Self {
    std::atomic<bool> is_ready;
    Client *client = nullptr;

    std::string api_key;
    std::string api_secret;

    unsigned long long reqid = 0;

    ~Self() {
        if (client) { delete client; }
    }
};
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

    if (self.client) { delete self.client; }
    self.client = new Client;
    self.client->on_open = std::function<void()>([this]() {
        on_open();
    });
    self.client->on_close = std::function<void()>([this]() {
        on_close();
    });
    self.client->on_message = std::function<void(std::string const &msg)>([this](std::string const &msg) {
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

    std::string url;
    if (is_test) {
        url = config.value("trade_test_url", "");
    } else {
        url = config.value("trade_url", "");
    }
    spdlog::info("{}", LOGHEAD);
    self.client->connect(url);
}

bool BybitTrade::is_ready() {
    return self.is_ready.load();
}

TradeOperateResult BybitTrade::order(core::datas::OrderObj const &order) {

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
    json_obj["args"] = {
        self.api_key,
        core::time::Time::now_millisecond() + 5000,
        ""
    };

    std::string json_str = json_obj.dump();
    auto code = self.client->send(json_str);
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

}

#undef LOGHEAD