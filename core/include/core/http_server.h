#pragma once
#include <httplib.h>
#include "pimpl.h"


namespace core::http::server {

class HttpServer {
public:
    HttpServer();
    virtual ~HttpServer();

private:
    Self &self;
};

} // core::http::server