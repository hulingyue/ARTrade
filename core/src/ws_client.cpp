#include "ws_client.h"

#include <iostream>
#include <ctime>
// #include <format>

// spdlog
#include "spdlog.h"

// websocketpp
#include "config/asio_client.hpp"
#include "client.hpp"

namespace {

using namespace std::chrono_literals;
typedef websocketpp::client<websocketpp::config::asio_tls_client> WSClient;
typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

struct Self {
    std::string_view uri;
    bool is_security = false;    

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
    
Client::Client() : self {*new Self{}} {

}

Client::~Client() {
    if (&self) { delete &self; }
}

void Client::set_reconnect(int second) {

}

int Client::send(std::string const &data) {
    return 0;
}

int Client::connect(std::string const &uri, bool is_security = false, int timeout = 0) {

    if (self.thread.joinable()) {
        return -1;
    }

    self.uri = uri;
    self.is_security = is_security;

    self.thread = std::thread([&](){ background(self); } );
    
}

void Client::stop() {
    
}

inline std::string log_head(std::string func) {
    // return std::format("[WS_Client::{}]", func);
    return func;
}

static context_ptr on_tls_init() {

    context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(
            boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::no_sslv3 |
            boost::asio::ssl::context::single_dh_use
        );
    }
    catch(const std::exception& e) {
        spdlog::error("{} {}", log_head(__func__), e.what());
        return nullptr;
    }

    return ctx;

}

static void background(Self &self) {

    do {
        if (time(0) - self.ts >= self.interval) {
            ws_connect(self);
        } else {
            std::this_thread::sleep_for(100ms);
        }
    } while (self.run && self.interval);

}

static void ws_connect(Self &self) {
    auto uri = self.uri;
    WSClient client;

    // set(self, client);
}

} // namespace WebSocket
} // namespace core 
