#pragma once
#include <string>
#include <httplib.h>


namespace core::http::util {

static inline std::string err_string(httplib::Error& err) {
    switch (err) {
    case httplib::Error::Success:
        return "Success";
    case httplib::Error::Unknown:
        return "Unknown";
    case httplib::Error::Connection:
        return "Connection";
    case httplib::Error::BindIPAddress:
        return "BindIPAddress";
    case httplib::Error::Read:
        return "Read";
    case httplib::Error::Write:
        return "Write";
    case httplib::Error::ExceedRedirectCount:
        return "ExceedRedirectCount";
    case httplib::Error::Canceled:
        return "Canceled";
    case httplib::Error::SSLConnection:
        return "SSLConnection";
    case httplib::Error::SSLLoadingCerts:
        return "SSLLoadingCerts";
    case httplib::Error::SSLServerVerification:
        return "SSLServerVerification";
    case httplib::Error::UnsupportedMultipartBoundaryChars:
        return "UnsupportedMultipartBoundaryChars";
    case httplib::Error::Compression:
        return "Compression";
    case httplib::Error::ConnectionTimeout:
        return "ConnectionTimeout";
    case httplib::Error::ProxyConnection:
        return "ProxyConnection";

    case httplib::Error::SSLPeerCouldBeClosed_:
        return "SSLPeerCouldBeClosed_";
    default:
        break;
    }

    return "";
}

} // namespace namespace core::http::util
