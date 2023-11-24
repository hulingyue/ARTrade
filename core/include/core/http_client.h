#pragma once
#include <httplib.h>
#include "pimpl.h"


namespace core::http::client {

class HttpClient {
public:
    HttpClient();
    virtual ~HttpClient();

public:
    HttpClient set_base_uri(std::string uri);
    HttpClient set_protocol(std::string protocol);
    HttpClient set_host(std::string host);
    HttpClient set_port(int port);
    HttpClient set_header(httplib::Headers headers);

    httplib::Result get(std::string path);
    httplib::Result post(std::string path, httplib::Params params);

private:
    std::string generate_uri(std::string path);

private:
    Self &self;
};

} // core::http::client