#include "ws_server.h"

namespace {
struct Self {
};
} // namespace


namespace core {
namespace WebSocket {
namespace Server {
    
Server::Server() : self {*new Self{}} {}
Server::~Server() {
    if (&self) { delete &self; }
}


} // namespace Server
} // namespace WebSocket
} // namespace core 
