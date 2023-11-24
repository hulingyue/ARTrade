#include <filesystem>
#include "core/http_client.h"


namespace {
struct Self {
    std::string uri;
    std::string protocol;
    std::string host;
    int port = 80;
    httplib::Headers headers;

    httplib::Client* client = nullptr;
};
}


namespace core::http::client {

HttpClient::HttpClient() : self( *new Self {} ) {

}

HttpClient::~HttpClient() {
    if (self.client) { delete self.client; }
    // if (&self) delete &self;
}

HttpClient HttpClient::set_base_uri(std::string uri) {
    self.uri = uri;
    return *this;
}

HttpClient HttpClient::set_protocol(std::string protocol) {
    self.protocol = protocol;
    if (self.protocol.length() > 0 && self.host.length() > 0) {
        self.uri = (std::filesystem::path(self.protocol) / self.host).string();
    }
    return *this;
}

HttpClient HttpClient::set_host(std::string host) {
    self.host = host;
    if (self.protocol.length() > 0 && self.host.length() > 0) {
        self.uri = (std::filesystem::path(self.protocol) / self.host).string();
    }
    return *this;
}

HttpClient HttpClient::set_port(int port) {
    self.port = port;
    return *this;
}

HttpClient HttpClient::set_header(httplib::Headers headers) {
    self.headers = headers;
    return *this;
}

httplib::Result HttpClient::get(std::string path) {
    if (self.client == nullptr) {
        self.client = new httplib::Client(self.host, self.port);
    }

    return self.client->Get(generate_uri(path), self.headers);
}

httplib::Result HttpClient::post(std::string path, httplib::Params params) {
    if (self.client == nullptr) {
        self.client = new httplib::Client(self.host, self.port);
    }

    return self.client->Post(generate_uri(path), self.headers, params);
}

inline std::string HttpClient::generate_uri(std::string path) {
    return (std::filesystem::path(self.uri) / path).string();
}

} // core::http::client