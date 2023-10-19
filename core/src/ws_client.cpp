#include "ws_client.h"

#include <iostream>
#include <ctime>

// spdlog
#include <spdlog/spdlog.h>

// websocketpp
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#define LOGHEAD ("[WS_Client::" + std::string(__func__) + "]")

namespace {

using namespace std::chrono_literals;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

struct Self {
    std::string uri;
    bool is_security = false;    
    core::WebSocket::WSEvent *ws_event = nullptr;

    time_t interval = 3;
    time_t ts = 0;

    websocketpp::connection_hdl handle;
    std::thread thread;
    std::mutex mutex;

    std::atomic<bool> run = true;
};

} // namespace


namespace core {
namespace WebSocket {

static void background(Self &self) {

    do {
        if (time(0) - self.ts >= self.interval) {
            // ws_connect(self);
        } else {
            std::this_thread::sleep_for(100ms);
        }
    } while (self.run && self.interval);

}

static void ws_connect(Self &self) {
    WSClient client;
    
    client.init_asio();
    client.set_open_handler(bind(&WSEvent::on_open, self.ws_event, ::_1));
    client.set_close_handler(bind(&WSEvent::on_close, self.ws_event, ::_1));
    client.set_fail_handler(bind(&WSEvent::on_fail, self.ws_event, ::_1));
    // client.set_ping_handler(bind(&WSEvent::on_ping, self.ws_event, ::_1, ::_2));
    client.set_pong_handler(bind(&WSEvent::on_pong, self.ws_event, ::_1, ::_2));
    client.set_message_handler(bind(&WSEvent::on_message, self.ws_event, ::_1, ::_2));

    client.set_tls_init_handler([](websocketpp::connection_hdl) {
        return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
    });

    websocketpp::lib::error_code ec;
    WSClient::connection_ptr con = client.get_connection(self.uri, ec);
    if (ec) {
        std::cout << "Could not create connection: " << ec.message() << std::endl;
        return;
    }
    
    client.connect(con);
    client.run();

    // set(self, client);
}

Client::Client() : self {*new Self{}} {

}

Client::~Client() {
    if (&self) { delete &self; }
}

Client* Client::set_event_handler(WSEvent *ws_event) {
    if (self.ws_event) {
        delete self.ws_event;
    }

    self.ws_event = ws_event;
    return this;
}

Client* Client::set_reconnect(int second) {
    spdlog::info("{} second: {}", log_head(__func__), second);
    return this;
}

int Client::send(std::string const &data) {
    spdlog::info("{} data: {}", log_head(__func__), data);
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
    spdlog::info("{}", log_head(__func__));
}

} // namespace WebSocket
} // namespace core 
