#include "binance.h"
#include "config.h"
#include <nlohmann/json.hpp>


#define LOGHEAD "[Binance::" + std::string(__func__) + "]"


namespace {
struct Self {
    BinanceMarket market;
};
} // namespace


Binance::Binance() : self { *new Self{} } {
    set_market_obj(&self.market);
}

Binance::~Binance() {

}

void Binance::init_config() {
    nlohmann::json binance_config = nlohmann::json::parse(
        R"(
            {
                "project": "Binance",
                "version:": 1.0
            }
        )"
    );

    core::config::Config::init(binance_config);
    spdlog::info("{} config: \n{}", LOGHEAD, core::config::Config::get().dump(4));
}

BinanceMarket& Binance::market() {
    return self.market;
}

#undef LOGHEAD