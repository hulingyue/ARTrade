#include <filesystem>
#include <regex>
#include <spdlog/spdlog.h>
#include "core/http_client.h"

#define LOGHEAD "[HttpClient::" + std::string(__func__) + "]"


namespace {
struct Self {
    std::string host;
    int port = 80;
    httplib::Headers headers;

    httplib::SSLClient* client = nullptr;
};
}


namespace core::http::client {

HttpClient::HttpClient() : self( *new Self {} ) {

}

HttpClient::~HttpClient() {
    if (self.client) { delete self.client; }
    // if (&self) delete &self;
}

HttpClient HttpClient::set_base_uri(const std::string uri) {
    std::regex rgx("(https?://)?([^/]+)");
    std::smatch match;

    if (std::regex_search(uri, match, rgx)) {
        self.host = match[2];
    }
    
    spdlog::info("{} uri: {} host: {}", LOGHEAD, uri, self.host);
    return *this;
}

HttpClient HttpClient::set_host(const std::string host) {
    self.host = host;
    spdlog::info("{} host: {}", LOGHEAD, self.host);
    return *this;
}

HttpClient HttpClient::set_port(const int port) {
    self.port = port;
    spdlog::info("{} port: {}", LOGHEAD, self.port);
    return *this;
}

HttpClient HttpClient::set_header(const httplib::Headers headers) {
    spdlog::info("{}", LOGHEAD);
    self.headers = headers;
    return *this;
}

HttpClient HttpClient::update_header(const httplib::Headers headers) {
    for (auto it = headers.begin(); it != headers.end(); it++) {
        auto range = self.headers.equal_range(it->first);
        for (auto r_it = range.first; r_it != range.second; r_it++) {
            r_it->second = it->second;
        }
    }
    return *this;
}

httplib::Result HttpClient::get(const std::string path) {
    spdlog::info("{}", LOGHEAD);
    if (self.client == nullptr) {
        self.client = new httplib::SSLClient(self.host, self.port);
    }

    return self.client->Get(path, self.headers);
}

httplib::Result HttpClient::get(const std::string path, const httplib::Params params) {
    spdlog::info("{}", LOGHEAD);
    if (self.client == nullptr) {
        self.client = new httplib::SSLClient(self.host, self.port);
    }

    return self.client->Get(path, params, self.headers);
}

httplib::Result HttpClient::post(const std::string path, const httplib::Params params) {
    spdlog::info("{}", LOGHEAD);
    if (self.client == nullptr) {
        self.client = new httplib::SSLClient(self.host, self.port);
    }

    return self.client->Post(path, self.headers, params);
}

} // core::http::client

#undef LOGHEAD