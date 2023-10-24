#include "bybit.h"
#include "config.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>


#define LOGHEAD "[Bybit::" + std::string(__func__) + "]"

namespace {
struct Self {
    BybitMarket market;
};
}


Bybit::Bybit() : self { *new Self{} } {
    set_market_obj(&self.market);
}

Bybit::~Bybit() {

}

void Bybit::init_config() {
    nlohmann::json bybit_config = nlohmann::json::parse(
        R"(
            {
                "project": "Bybit",
                "version:": 1.0
            }
        )"
    );

    core::config::Config::init(bybit_config);
    spdlog::info("{} config: \n{}", LOGHEAD, core::config::Config::get().dump(4));
}

BybitMarket& Bybit::market() {
    return self.market;
}

#undef HEADLOG