#include "core/http_client.h"


namespace {
struct Self {
};
}


namespace core::http::client {

HttpClient::HttpClient() : self( *new Self {} ) {

}

HttpClient::~HttpClient() {
    delete &self;
}

} // core::http::client