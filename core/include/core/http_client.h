#pragma once
#include <httplib.h>
#include "pimpl.h"


namespace core::http::client {

class HttpClient {
public:
    HttpClient();
    virtual ~HttpClient();

private:
    Self &self;
};

} // core::http::client