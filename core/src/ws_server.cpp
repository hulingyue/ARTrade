#include "core/ws_server.h"

namespace {
struct Self {
};
} // namespace


namespace core::WebSocket::Server {
    
Server::Server() : self {*new Self{}} {}
Server::~Server() {
    if (&self) { delete &self; }
}


} // namespace core::WebSocket::Server 
