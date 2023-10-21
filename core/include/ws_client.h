#pragma once 
#include <iostream>
#include <functional>

#include <spdlog/spdlog.h>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include "pimpl.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> WSSClient;
typedef websocketpp::client<websocketpp::config::asio_client> WSClient;
typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

namespace core {
namespace WebSocket {
namespace Client {
    
class Client {
public:
    Client();
    ~Client();

    std::function<void()> on_open;
    std::function<void()> on_close;
    std::function<void()> on_fail;
    std::function<void(std::string const &msg)> on_message;

    Client* set_reconnect(int second);

    int send(std::string const &data);

    int connect(std::string const &uri, int timeout = 0);

    void stop();

private:
    Self &self;
};

} // namespace Client
} // namespace name
} // namespace cor 
