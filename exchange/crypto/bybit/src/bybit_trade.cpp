#include <cassert>
#include "bybit_trade.h"
#include <core/ws_client.h>
#include <core/http/util.hpp>
#include <core/http/client.h>
#include <core/util.h>
#include <core/time.hpp>
#include <nlohmann/json.hpp>
#include <core/order.hpp>

#include "format.hpp"
#include "signature.hpp"


#define LOGHEAD "[BybitTrade::" + std::string(__func__) + "]"

namespace {
struct Self {
    std::atomic<bool> is_ready;
    core::websocket::client::WebSocketClient *websocket_client = nullptr;
    core::http::client::HttpClient http_client;
    int interval = 0;

    std::string api_key;
    std::string api_secret;

    std::string category;

    unsigned long long reqid = 0;

    ~Self() {
        if (websocket_client) { delete websocket_client; }
    }
};

void update_headers(Self *self, const nlohmann::json& parameters, const std::string method) {
    std::string timestamp = std::to_string(core::time::Time().to_milliseconds());

    std::string sign;
    static const std::string recv_window = "5000";
    if (method == "GET") {
        sign = restful_get_signature(timestamp, std::move(self->api_secret), self->api_key, recv_window, parameters);
    } else if (method == "POST") {
        sign = restful_post_signature(timestamp, std::move(self->api_secret), self->api_key, recv_window, parameters);
    } else {
        spdlog::error("{} unsupport this method: {}", LOGHEAD, method);
        return;
    }

    httplib::Headers headers = {
        {"X-BAPI-SIGN", sign},
        {"X-BAPI-API-KEY", self->api_key},
        {"X-BAPI-TIMESTAMP", timestamp},
        {"X-BAPI-RECV-WINDOW", recv_window}
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

TradeOperateResult BybitTrade::order(core::datas::OrderObj &order) {
    std::string path = "/v5/order/create";

    TradeOperateResult result {
        .code = -1,
        .msg = ""
    };

    bool is_open = true;
    if (order.offset == core::datas::OrderOffset::OPEN) {
        is_open = false;
    } else if (order.offset == core::datas::OrderOffset::CLOSE
        || order.offset == core::datas::OrderOffset::CLOSETODAY
        || order.offset == core::datas::OrderOffset::CLOSEYESTERDAY
    ) {
        is_open = true;
    } else {
        result.msg = "unknow offset!";
        return result;
    }

    nlohmann::json parameters = {
        {"orderLinkId", std::to_string(order.client_id)},
        {"category", self.category},
        {"symbol", std::string(order.symbol)},
        {"side", side_to_bybit(order.side)},
        {"orderType", type_to_bybit(order.type)},
        {"qty", std::to_string(order.quantity)},
        {"price", std::to_string(order.price)},
        {"timeInForce", tif_to_bybit(order.tif)},
        {"reduceOnly", is_open}
    };

    update_headers(&self, parameters, "POST");
    httplib::Result res = self.http_client.post(path, parameters.dump());

    if (res) {
        result.code = res->status;
        result.msg = res->body;

        if (res->status == 200) {
            // {"retCode":10004,"retMsg":"error sign! origin_string[1701180667626dHPRDHOkSSKFEC4W0m5000category=linear\u0026orderType=Market\u0026price=28000.000000\u0026qty=0.010000\u0026side=Buy\u0026symbol=BTCUSDT\u0026timeInForce=GTC]","result":{},"retExtInfo":{},"time":1701180668273}
            nlohmann::json j_msg = nlohmann::json::parse(res->body);
            result.code = j_msg.value("retCode", 0);
            result.msg = j_msg.value("retMsg", "");
            if (result.code == 0) {
                std::string order_id = j_msg.value("result", nlohmann::json::object()).value("orderId", "");
                std::string order_link_id = j_msg.value("result", nlohmann::json::object()).value("orderLinkId", "0");
                memcpy(order.order_id, order_id.c_str(), core::datas::ORDERID_MAX_SIZE);
                order.order_id[core::datas::ORDERID_MAX_SIZE - 1] = '\0';
            }
        }

        if (result.code == 0) {
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

TradeOperateResult BybitTrade::cancel(core::datas::CancelObj &order) {
    std::string path = "/v5/order/cancel";

    nlohmann::json parameters = {
        {"category", self.category},
        {"symbol", std::string(order.symbol)},
        {"orderLinkId", order.order_id}
    };

    update_headers(&self, parameters, "POST");
    
    TradeOperateResult result {
        .code = -1,
        .msg = ""
    };
    httplib::Result res = self.http_client.post(path, parameters.dump());

    return result;
}

void BybitTrade::interval_1s() {
    self.interval += 1;
    if (self.interval % 30 == 0) {
        ping();
    }
}

void BybitTrade::on_open() {
    // self.is_ready.store(true);
    nlohmann::json json_obj;
    json_obj["req_id"] = self.reqid;
    json_obj["op"] = "auth";

    auto exipres = core::time::Time().to_milliseconds() + 1000;
    json_obj["args"] = {
        self.api_key,
        exipres,
        websocket_signature(exipres, self.api_secret)
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

    std::string topic = message.value("topic", "");
    if (topic.length() > 0) {
        spdlog::info("{} topic: {} msg: {}", LOGHEAD, topic, msg);
        uint64_t creation_time = message["creationTime"].get<uint64_t>();
        nlohmann::json datas = message["data"];

        for (auto &data: datas) {
            if (topic == "execution") { // 成交
                std::string order_id = data["orderId"].get<std::string>();
                std::string client_id = data["orderLinkId"].get<std::string>();
                auto obj = core::order::Order::get_instance()->find(client_id);
                if (obj == nullptr) { return; }

                double exec_price = std::stod(data["execPrice"].get<std::string>());
                double exec_quantity = std::stod(data["execQty"].get<std::string>());

                obj->avg_price = ((obj->avg_price * obj->traded) + (exec_price * exec_quantity)) / (obj->traded + exec_quantity);
                obj->traded = obj->traded + exec_quantity;
                obj->status = core::datas::OrderStatus::PARTIALLYFILLED;

                continue;
            } else if (topic == "order") { // 订单
                std::string client_id = data["orderLinkId"].get<std::string>();
                auto obj = core::order::Order::get_instance()->find(client_id);
                if (obj == nullptr) { return; }

                std::string status = data["orderStatus"].get<std::string>();
                if (status == "New") {
                    obj->status = core::datas::OrderStatus::ACCEPTED;
                } else if (status == "PartiallyFilled") {
                    obj->status = core::datas::OrderStatus::PARTIALLYFILLED;
                } else if (status == "Filled") {
                    obj->status = core::datas::OrderStatus::FILLED;
                } else if (status == "Cancelled" && status == "PartiallyFilledCanceled") {
                    obj->status = core::datas::OrderStatus::CANCEL;
                } else if (status == "Rejected") {
                    obj->status = core::datas::OrderStatus::REJECTED;
                }

                obj->traded = std::stod(data["cumExecQty"].get<std::string>());
                std::string avg_price = data["avgPrice"].get<std::string>();
                avg_price = !avg_price.empty() ? avg_price : "0";
                obj->avg_price = std::stod(avg_price);

                continue;
            } else if (topic == "position") { // 仓位
                continue;
            } else if (topic == "wallet") { // 钱包
                continue;
            } else if (topic == "greeks") { // 期权相关
                continue;
            }
        }
        return;
    }

    std::string op = message.value("op", "");
    if (op.length() > 0) {
        bool status = message.value("success", false);
        if (op == "auth" && status) {
            // subscribe private message
            subscribe_private_channel();
        } else if (op == "subscribe") {
            self.is_ready.store(status);
        } else if (op == "pong") { return; }

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

void BybitTrade::subscribe_private_channel() {
    std::string json_obj = R"({
        "op": "subscribe",
        "args": [
            "position",
            "execution",
            "order",
            "wallet",
            "greeks"
        ]
    })";
    self.websocket_client->send(json_obj);
}

#undef LOGHEAD