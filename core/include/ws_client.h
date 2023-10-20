#pragma once 
#include <iostream>
#include <functional>
#include "pimpl.h"
#include "ws_event.h"

namespace core {
namespace WebSocket {
namespace Client {
    
class Client {
public:
    Client();
    ~Client();

    Client* set_event_handler(WSEvent *ws_event);

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
