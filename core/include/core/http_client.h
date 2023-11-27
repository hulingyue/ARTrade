#pragma once
#ifndef CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif
#include <httplib.h>
#include "pimpl.h"


namespace core::http::client {

class HttpClient {
public:
    HttpClient();
    virtual ~HttpClient();

public:
    HttpClient set_uri(const std::string uri);
    HttpClient set_host(const std::string host);
    HttpClient set_port(const int port);
    HttpClient set_header(const httplib::Headers headers);

    HttpClient update_header(const httplib::Headers headers);

    httplib::Result get(const std::string path);
    httplib::Result get(const std::string path, const httplib::Params params);
    httplib::Result post(const std::string path, const httplib::Params params);

private:
    Self &self;
};

} // core::http::client