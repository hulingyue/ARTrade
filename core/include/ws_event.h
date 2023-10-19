#pragma once

#include <iostream>
#include <spdlog/spdlog.h>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#define LOGHEAD ("[WSEvent::" + std::string(__func__) + "]")

namespace core {
namespace WebSocket {
    
    typedef websocketpp::client<websocketpp::config::asio_tls_client> WSSClient;
    typedef websocketpp::client<websocketpp::config::asio_client> WSClient;
    typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;
    typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
    
    struct WSEvent {        

        void on_open(websocketpp::connection_hdl hdl) {
            spdlog::info("{}", LOGHEAD);
        }

        void on_close(websocketpp::connection_hdl hdl) {
            spdlog::info("{}", LOGHEAD);
        }

        void on_fail(websocketpp::connection_hdl hdl) {
            spdlog::error("{}", LOGHEAD);
        }

        void on_ping(websocketpp::connection_hdl hdl, std::string msg) {
            spdlog::info("{} msg: {}", LOGHEAD, msg);
        }

        void on_pong(websocketpp::connection_hdl hdl, std::string msg) {
            spdlog::info("{} msg: {}", LOGHEAD, msg);
        }

        void on_message(websocketpp::connection_hdl hdl, WSClient::message_ptr msg) {
            spdlog::info("{} Received message: {}", LOGHEAD, msg->get_payload());
        }



    };

} // namespace WebSocket
} // namespace core
