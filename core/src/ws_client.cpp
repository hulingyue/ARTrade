#include "ws_client.h"

#include <iostream>
#include <ctime>

// spdlog
#include <spdlog/spdlog.h>

// websocketpp
// #include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace {

using namespace std::chrono_literals;
// typedef websocketpp::client<websocketpp::config::asio_tls_client> WSClient;
// typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;
// typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

struct Self {
    std::string_view uri;
    bool is_security = false;    

    time_t interval = 3;
    time_t ts = 0;

    // websocketpp::connection_hdl handle;
    std::thread thread;
    std::mutex mutex;

    std::atomic<bool> run = true;
};

} // namespace


namespace core {
namespace WebSocket {

inline std::string log_head(std::string func) {
    return "[WS_Client::" + func + "]";
}

// static context_ptr on_tls_init() {

//     context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

//     try {
//         ctx->set_options(
//             boost::asio::ssl::context::default_workarounds |
//             boost::asio::ssl::context::no_sslv2 |
//             boost::asio::ssl::context::no_sslv3 |
//             boost::asio::ssl::context::single_dh_use
//         );
//     }
//     catch(const std::exception& e) {
//         spdlog::error("{} {}", log_head(__func__), e.what());
//         return nullptr;
//     }

//     return ctx;

// }

static void background(Self &self) {

    // do {
    //     if (time(0) - self.ts >= self.interval) {
    //         ws_connect(self);
    //     } else {
    //         std::this_thread::sleep_for(100ms);
    //     }
    // } while (self.run && self.interval);

}

static void ws_connect(Self &self) {
    auto uri = self.uri;
    // WSClient client;

    // set(self, client);
}

Client::Client() : self {*new Self{}} {

}

Client::~Client() {
    if (&self) { delete &self; }
}

void Client::set_reconnect(int second) {
    spdlog::info("{} second: {}", log_head(__func__), second);
}

int Client::send(std::string const &data) {
    spdlog::info("{} data: {}", log_head(__func__), data);
    return 0;
}

int Client::connect(std::string const &uri, bool is_security, int timeout) {

    spdlog::info("{} uri: {} is_security: {} timeout: {}", log_head(__func__), uri, is_security, timeout);

    // if (self.thread.joinable()) {
    //     return -1;
    // }

    // self.uri = uri;
    // self.is_security = is_security;

    // self.thread = std::thread([&](){ background(self); } );
    return 0;
}

void Client::stop() {
    spdlog::info("{}", log_head(__func__));
}

} // namespace WebSocket
} // namespace core 
