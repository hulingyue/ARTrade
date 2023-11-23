#include "bybit.h"
#include <core/config.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>


#define LOGHEAD "[Bybit::" + std::string(__func__) + "]"

namespace {
struct Self {
    BybitMarket market;
};
}


Bybit::Bybit(int argc, char** argv) : Modules(argc, argv), self { *new Self{} } {
    set_market_obj(&self.market);
}

Bybit::~Bybit() {

}

std::string Bybit::project_name() {
    return "Bybit";
}

core::datas::MessageType Bybit::message_type() {
    return core::datas::MessageType::ShareMemory;
}

void Bybit::init_config() {
    nlohmann::json bybit_config = nlohmann::json::parse(
        R"(
            {
                "log_level": "debug",
                "project": "Bybit",
                "version:": 1.0,
                "is_spot": true,
                "is_test": true,
                "market_test_spot_url": "wss://stream-testnet.bybit.com/v5/public/spot",
                "market_test_future_url": "wss://stream-testnet.bybit.com/v5/public/linear",
                "market_spot_url": "wss://stream.bybit.com/v5/public/spot",
                "market_future_url": "wss://stream.bybit.com/v5/public/linear",
                "trade_test_url": "wss://stream-testnet.bybit.com/v5/private",
                "trade_url": "wss://stream.bybit.com/v5/private",
                "trade_test_restful_url": "https://api-testnet.bybit.com",
                "trade_restful_url": "https://api.bybit.com"
            }
        )"
    );

    core::config::Config::init(bybit_config);
    spdlog::info("{} config: \n{}", LOGHEAD, core::config::Config::get().dump(4));
}

void Bybit::interval_1s() {
    self.market.interval_1s();
}

BybitMarket& Bybit::market() {
    return self.market;
}

#undef HEADLOG