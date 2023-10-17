#pragma once 
#include <iostream>
#include <functional>
#include "pimpl.h"

namespace core {
namespace WebSocket {
    
class Server {
public:
    Server();
    ~Server();

    std::function<void(Server &)> on_open;
    std::function<void(Server &)> on_close;
    std::function<void(Server &)> on_fail;
    std::function<void(Server &, std::string_view msg)> on_ping;
    std::function<void(Server &)> on_pong;
    std::function<void(Server &, std::string const &msg)> on_message;

private:
    Self &self;
};

} // namespace name
    
} // namespace cor 
