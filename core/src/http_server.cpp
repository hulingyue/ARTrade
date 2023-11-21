#include "core/http_server.h"


namespace {
struct Self {

};
}


namespace core::http::server {

HttpServer::HttpServer() : self ( *new Self ) {

}

HttpServer::~HttpServer() {
    delete &self;
}

} // namespace core::http::server
