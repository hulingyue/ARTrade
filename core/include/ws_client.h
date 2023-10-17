#pragma once 
#include <iostream>
#include <functional>
#include "pimpl.h"

namespace core {
namespace WebSocket {
    
class Client {
public:
    Client();
    ~Client();

    std::function<void(Client &)> on_open;
    std::function<void(Client &)> on_close;
    std::function<void(Client &)> on_fail;
    std::function<void(Client &, std::string_view msg)> on_ping;
    std::function<void(Client &)> on_pong;
    std::function<void(Client &, std::string const &msg)> on_message;

    void set_reconnect(int second);

    int send(std::string const &data);

    int connect(std::string const &uri, bool is_security = false, int timeout = 0);

    void stop();

private:
    Self &self;
};

} // namespace name
} // namespace cor 
