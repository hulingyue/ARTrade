#include <iostream>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
}

int main() {
    std::string url = "wss://stream.binance.com:9443/ws/btcusdt@trade";
    
    client c;
    c.init_asio();
    c.set_message_handler(&on_message);
    
    // 设置SSL选项
    c.set_tls_init_handler([](websocketpp::connection_hdl) {
        return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
    });
    
    websocketpp::lib::error_code ec;
    client::connection_ptr con = c.get_connection(url, ec);
    if (ec) {
        std::cout << "Could not create connection: " << ec.message() << std::endl;
        return 0;
    }
    
    c.connect(con);
    c.run();
    
    return 0;
}
