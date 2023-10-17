#include "ws_server.h"

namespace {
struct Self {
};
} // namespace


namespace core {
namespace WebSocket {
    
Server::Server() : self {*new Self{}} {}
Server::~Server() {
    if (&self) { delete &self; }
}


} // namespace WebSocket
} // namespace core 
