#pragma once 
#include <iostream>
#include <functional>
#include "pimpl.h"
#include "ws_event.h"

namespace core {
namespace WebSocket {
    
class Client {
public:
    Client();
    ~Client();

    Client* set_event_handler(WSEvent *ws_event);

    Client* set_reconnect(int second);

    int send(std::string const &data);

    int connect(std::string const &uri, bool is_security = false, int timeout = 0);

    void stop();

private:
    Self &self;
};

} // namespace name
} // namespace cor 
