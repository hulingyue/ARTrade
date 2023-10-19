#pragma once

#include <iostream>
#include <spdlog/spdlog.h>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace core {
namespace WebSocket {
    
    typedef websocketpp::client<websocketpp::config::asio_tls_client> WSSClient;
    typedef websocketpp::client<websocketpp::config::asio_client> WSClient;
    typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;
    typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
    
    struct WSEvent {
        
        inline static std::string log_head(std::string func) {
            return "[WSEvent::" + func + "]";
        }

        void on_open(websocketpp::connection_hdl hdl) {
            spdlog::info("{}", log_head(__func__));
        }

        void on_close(websocketpp::connection_hdl hdl) {
            spdlog::info("{}", log_head(__func__));
        }

        void on_fail(websocketpp::connection_hdl hdl) {
            spdlog::error("{}", log_head(__func__));
        }

        void on_ping(websocketpp::connection_hdl hdl, std::string msg) {
            spdlog::info("{} msg: {}", log_head(__func__), msg);
        }

        void on_pong(websocketpp::connection_hdl hdl, std::string msg) {
            spdlog::info("{} msg: {}", log_head(__func__), msg);
        }

        void on_message(websocketpp::connection_hdl hdl, WSClient::message_ptr msg) {
            spdlog::info("{} Received message: {}", log_head(__func__), msg->get_payload());
        }



    };

} // namespace WebSocket
} // namespace core
