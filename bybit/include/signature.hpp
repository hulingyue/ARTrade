#include <string>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include <nlohmann/json.hpp>


namespace {
static std::string generate_query_string(const nlohmann::json& parameters) {
    std::ostringstream oss;
    for (const auto& item : parameters.items()) {
        std::string key = item.key();
        std::string value = item.value().get<std::string>();
        oss << key << "=" << value << "&";
    }
    std::string result = oss.str();
    return result.substr(0, result.length() - 1); // Remove trailing '&'
}

static std::string signature(std::string&& api_secret, std::string&& msg) {
    unsigned char* digest = HMAC(EVP_sha256(), api_secret.c_str(), api_secret.length(), reinterpret_cast<const unsigned char*>(msg.c_str()), msg.length(), nullptr, nullptr);
    char mdString[SHA256_DIGEST_LENGTH*2+1];
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    return std::string(mdString);
}
}

static std::string websocket_signature(uint64_t expires, std::string& api_secret) {
    std::string msg = "GET/realtime" + std::to_string(expires);
    return signature(std::move(api_secret), std::move(msg));
}

static std::string restful_get_signature(
    const std::string& timestamp,
    std::string&& api_secret,
    const std::string& api_key,
    const std::string& recv_window,
    const nlohmann::json& parameters
    ) {
    std::string queryString = generate_query_string(parameters);
    std::string msg = timestamp + api_key + recv_window +  queryString;
    return signature(std::move(api_secret), std::move(msg));
}

static std::string restful_post_signature(
    const std::string& timestamp,
    std::string&& api_secret,
    const std::string& api_key,
    const std::string& recv_window,
    const nlohmann::json& parameters
    ) {
    std::string paramJson = parameters.dump();
    std::string msg = timestamp + api_key + recv_window + paramJson;
    return signature(std::move(api_secret), std::move(msg));
}
