#include "core/ws_client.h"

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

    WSSClient *wss_client = nullptr;
    WSClient *ws_client = nullptr;

    core::websocket::client::WebSocketClient *obj;
    core::websocket::client::WebSocketClient *operator->() { return obj; }
};

} // namespace


namespace core::websocket::client {

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
            self->on_open();
        } else {
            spdlog::info("{}", LOG_HEAD("on_open"));
        }
    });

    client.set_close_handler([&self](websocketpp::connection_hdl hdl) {
        if (self->on_close) {
            self->on_close();
        } else {
            spdlog::info("{}", LOG_HEAD("on_close"));
        }
    });

    client.set_fail_handler([&self](websocketpp::connection_hdl hdl) {
        if (self->on_fail) {
            self->on_fail();
        } else {
            spdlog::info("{}", LOG_HEAD("on_fail"));
        }
    });

    client.set_message_handler([&self](websocketpp::connection_hdl hdl, WSClient::message_ptr msg) {
        if (self->on_message) {
            self->on_message(msg->get_payload());
        } else {
            spdlog::info("{} msg: {}", LOG_HEAD("on_message"), msg->get_payload());
        }
    });

    self.ts = time(0);
    websocketpp::lib::error_code ec;
    auto con = client.get_connection(self.uri, ec);
    if (ec) {
        spdlog::error("{} Could not create connection: {}", LOGHEAD, ec.message());
        return;
    }

    client.connect(con);
    spdlog::info("{} Successful connected, ready to run!", LOGHEAD);
    
    self.handle = con->get_handle();
    client.run();
}

static void background(Self &self) {

    do {
        if (time(0) - self.ts >= self.interval) {
            if (self.is_security) {
                WSSClient client;
                set_sls_init(client);
                self.wss_client = &client;
                pp_connect(client, self);
            } else {
                WSClient client;
                self.ws_client = &client;
                pp_connect(client, self);
            }
        } else {
            std::this_thread::sleep_for(100ms);
        }
    } while (self.run.load() && self.interval);
    
    spdlog::info("{} backgound death!", LOGHEAD);

}

WebSocketClient::WebSocketClient() : self {*new Self{ .obj = this }} {

}

WebSocketClient::~WebSocketClient() {
    if (&self) { delete &self; }
}

WebSocketClient* WebSocketClient::set_reconnect(int second) {
    spdlog::info("{} second: {}", LOGHEAD, second);
    self.interval = second;
    return this;
}

int WebSocketClient::send(std::string const &data) {
    spdlog::info("{} data: {}", LOGHEAD, data);

    std::scoped_lock lock(self.mutex);

    try {
        if (self.is_security && self.wss_client != nullptr) {
            self.wss_client->send(self.handle, data, websocketpp::frame::opcode::text);
        } else if (!self.is_security && self.ws_client != nullptr) {
            self.ws_client->send(self.handle, data, websocketpp::frame::opcode::text);
        } else {
            spdlog::error("{} error code: -1 , error msg: {} client is nullptr ", LOGHEAD, self.is_security ? "wss" : "ws");
            return -1;
        }
    } catch(const std::exception& e) {
        spdlog::error("{} catch info: {}", LOGHEAD, std::string(e.what()));
    }
    
    return 0;
}

int WebSocketClient::connect(std::string const &uri, int timeout) {

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

    self.run.store(true);
    self.thread = std::thread([&](){ background(self); } );
    return 0;
}

void WebSocketClient::stop() {
    spdlog::info("{}", LOGHEAD);

    std::scoped_lock lock(self.mutex);

    try {
        if (self.is_security && self.wss_client != nullptr) {
            self.wss_client->close(self.handle, websocketpp::close::status::normal, "Closing connection");
        } else if (!self.is_security && self.ws_client != nullptr) {
            self.ws_client->close(self.handle, websocketpp::close::status::normal, "Closing connection");
        } else {
            spdlog::error("{} error code: -1 , error msg: {} client is nullptr ", LOGHEAD, self.is_security ? "wss" : "ws");
            return;
        }

        self.run.store(false);
    } catch(const std::exception& e) {
        spdlog::error("{} catch info: {}", LOGHEAD, std::string(e.what()));
    }
}

} // namespace core::websocket::client 

#undef LOGHEAD
#undef LOG_HEAD