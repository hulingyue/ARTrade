#pragma once 
#include <iostream>
#include <functional>
#include "pimpl.h"

namespace core::websocket::server {
    
class WebSocketServer {
public:
    WebSocketServer();
    ~WebSocketServer();

    std::function<void(WebSocketServer &)> on_open;
    std::function<void(WebSocketServer &)> on_close;
    std::function<void(WebSocketServer &)> on_fail;
    std::function<void(WebSocketServer &, std::string_view msg)> on_ping;
    std::function<void(WebSocketServer &)> on_pong;
    std::function<void(WebSocketServer &, std::string const &msg)> on_message;

private:
    Self &self;
};

} // namespace core::websocket::server 
