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

core::base::datas::MessageType Bybit::message_type() {
    return core::base::datas::MessageType::ShareMemory;
}

void Bybit::init_config() {
    nlohmann::json bybit_config = nlohmann::json::parse(
        R"(
            {
                "log_level": "debug",
                "project": "Bybit",
                "version:": 1.0
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