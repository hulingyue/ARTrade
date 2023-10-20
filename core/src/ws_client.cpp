#include "ws_client.h"

#include <iostream>
#include <ctime>

// spdlog
#include <spdlog/spdlog.h>

// websocketpp
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#define LOG_HEAD(func) ("[WS_Client::" + std::string(func) + "]")
#define LOGHEAD LOG_HEAD(__func__)

namespace {

using namespace std::chrono_literals;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

struct Self {
    std::string uri;
    bool is_security = false;

    time_t interval = 3;
    time_t ts = 0;

    websocketpp::connection_hdl handle;
    std::thread thread;
    std::mutex mutex;

    std::atomic<bool> run = true;

    core::WebSocket::Client::Client *obj;
    core::WebSocket::Client::Client *operator->() { return obj; }
};

} // namespace


namespace core {
namespace WebSocket {
namespace Client {

static inline void set_sls_init(WSSClient &client) {
    client.set_tls_init_handler([](websocketpp::connection_hdl) {
        return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
    });
}

template <typename T>
static void pp_connect(T &client, Self &self) {
    client.init_asio();
    client.set_open_handler([&self](websocketpp::connection_hdl hdl) {
        if (self->on_open) {
            self->on_open(*self.obj);
        } else {
            spdlog::info("{}", LOG_HEAD("on_open"));
        }
    });

    client.set_close_handler([&self](websocketpp::connection_hdl hdl) {
        if (self->on_close) {
            self->on_close(*self.obj);
        } else {
            spdlog::info("{}", LOG_HEAD("on_close"));
        }
    });

    client.set_fail_handler([&self](websocketpp::connection_hdl hdl) {
        if (self->on_fail) {
            self->on_fail(*self.obj);
        } else {
            spdlog::info("{}", LOG_HEAD("on_fail"));
        }
    });

    client.set_message_handler([&self](websocketpp::connection_hdl hdl, WSClient::message_ptr msg) {
        if (self->on_message) {
            self->on_message(*self.obj, msg->get_payload());
        } else {
            spdlog::info("{} msg: {}", LOG_HEAD("on_message"), msg->get_payload());
        }
    });

    websocketpp::lib::error_code ec;
    auto con = client.get_connection(self.uri, ec);
    if (ec) {
        spdlog::error("{} Could not create connection: {}", LOGHEAD, ec.message());
        return;
    }

    client.connect(con);
    spdlog::info("{} Successful connected, ready to run!", LOGHEAD);
    client.run();
}

static void background(Self &self) {

    do {
        if (time(0) - self.ts >= self.interval) {
            if (self.is_security) {
                WSSClient client;
                set_sls_init(client);
                pp_connect(client, self);
            } else {
                WSClient client;
                pp_connect(client, self);
            }
        } else {
            std::this_thread::sleep_for(100ms);
        }
    } while (self.run && self.interval);

}

Client::Client() : self {*new Self{ .obj = this }} {

}

Client::~Client() {
    if (&self) { delete &self; }
}

Client* Client::set_reconnect(int second) {
    spdlog::info("{} second: {}", LOGHEAD, second);
    return this;
}

int Client::send(std::string const &data) {
    spdlog::info("{} data: {}", LOGHEAD, data);
    return 0;
}

int Client::connect(std::string const &uri, int timeout) {

    spdlog::info("{} uri: {} timeout: {}", LOGHEAD, uri, timeout);

    if (self.thread.joinable()) {
        return -1;
    }

    self.uri = uri;

    if (self.uri.compare(0, 5, "ws://") == 0) {
        self.is_security = false;
    } else if (self.uri.compare(0, 6, "wss://") == 0) {
        self.is_security = true;
    } else {
        spdlog::error("{} Wrong uri!", LOGHEAD);
        return -1;
    }

    self.thread = std::thread([&](){ background(self); } );
    return 0;
}

void Client::stop() {
    spdlog::info("{}", LOGHEAD);
}

} // namespace Client
} // namespace WebSocket
} // namespace core 

#undef LOGHEAD
#undef LOG_HEAD