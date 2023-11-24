#include "core/ws_server.h"

namespace {
struct Self {
};
} // namespace


namespace core::websocket::server {
    
WebSocketServer::WebSocketServer() : self {*new Self{}} {}
WebSocketServer::~WebSocketServer() {
    if (&self) { delete &self; }
}


} // namespace core::websocket::server 
