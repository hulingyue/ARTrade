#include <filesystem>
#include <regex>
#include <spdlog/spdlog.h>
#include "core/http/client.h"

#define LOGHEAD "[HttpClient::" + std::string(__func__) + "]"


namespace {
struct Self {
    std::string host;
    httplib::Headers headers;

    httplib::SSLClient* ssl_client = nullptr;
};
}


namespace core::http::client {

HttpClient::HttpClient() : self( *new Self {} ) {

}

HttpClient::~HttpClient() {
    if (self.ssl_client) {
        delete self.ssl_client;
        self.ssl_client = nullptr;
    }
    if (&self) delete &self;
}

HttpClient& HttpClient::set_uri(const std::string uri) {
    std::regex rgx("(https?://)?([^/]+)");
    std::smatch match;

    if (std::regex_search(uri, match, rgx)) {
        self.host = match[2];
    }
    
    spdlog::info("{} uri: {} host: {}", LOGHEAD, uri, self.host);
    return *this;
}

HttpClient& HttpClient::set_host(const std::string host) {
    self.host = host;
    spdlog::info("{} host: {}", LOGHEAD, self.host);
    return *this;
}

HttpClient& HttpClient::set_header(const httplib::Headers headers) {
    spdlog::info("{}", LOGHEAD);
    self.headers = headers;
    return *this;
}

HttpClient& HttpClient::update_header(const httplib::Headers headers) {
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
    if (self.ssl_client == nullptr) {
        self.ssl_client = new httplib::SSLClient(self.host);
    }

    return self.ssl_client->Get(path, self.headers);
}

httplib::Result HttpClient::get(const std::string path, const httplib::Params params) {
    spdlog::info("{}", LOGHEAD);
    if (self.ssl_client == nullptr) {
        self.ssl_client = new httplib::SSLClient(self.host);
    }

    return self.ssl_client->Get(path, params, self.headers);
}

httplib::Result HttpClient::post(const std::string path, const httplib::Params params) {
    spdlog::info("{}", LOGHEAD);
    if (self.ssl_client == nullptr) {
        self.ssl_client = new httplib::SSLClient(self.host);
    }

    return self.ssl_client->Post(path, self.headers, params);
}

httplib::Result HttpClient::post(
      const std::string &path
    , const std::string &body
    , const std::string &content_type
    ) {
    spdlog::info("{}", LOGHEAD);
    if (self.ssl_client == nullptr) {
        self.ssl_client = new httplib::SSLClient(self.host);
    }

    return self.ssl_client->Post(path, self.headers, body, content_type);
}

} // core::http::client

#undef LOGHEAD