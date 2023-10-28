#include <filesystem>

#include "core/modules.h"
#include "core/config.h"
#include "core/util.h"


#define LOGHEAD "[Modules::" + std::string(__func__) + "]"

namespace {

struct Self {
    core::api::market::Market *market = nullptr;
    core::api::trade::Trade *trade = nullptr;

    ~Self() {
        if (market) { delete market; }
        if (trade) { delete trade; }
    }
};

}

namespace core {
namespace modules {

Modules::Modules() : self { *new Self{} } {

}

Modules::~Modules() {
    if (&self) { delete &self; }
}

// Market
void Modules::set_market_obj(core::api::market::Market *market_obj) {
    if (self.market) {
        delete self.market;
    }
    self.market = market_obj;
}

bool Modules::is_market_ready() {
    if (self.market) {
        return self.market->is_ready();
    }

    return false;
}

// Trade
void Modules::set_trade_obj(core::api::trade::Trade *trade_obj) {
    if (self.trade) {
        delete self.trade;
    }
    self.trade = trade_obj;
}

bool Modules::is_trade_ready() {
    if (self.trade) {
        return self.trade->is_ready();
    }
    return false;
}

// Config
void Modules::init_config() {
    default_config();
    spdlog::info("{} init config with default config!", LOGHEAD);
}

void Modules::default_config() {
    nlohmann::json default_config = nlohmann::json::parse(
        R"(
            {
                "project": "default",
                "version": 1.0
            }
        )"
    );

    core::config::Config::init(default_config);
}

// 
bool Modules::is_ready() {
    if (self.trade && self.market) {
        return self.trade->is_ready() && self.market->is_ready();
    } else if (self.trade) {
        return self.trade->is_ready();
    } else if (self.market) {
        return self.market->is_ready();
    } 
    return false;
}

void handle_sigint(int signal) {
    std::cout << std::endl;
    spdlog::info("{} Received Ctrl+C signal, program is about to exit!", LOGHEAD);
    
    exit(signal);
}


void Modules::run() {
    std::signal(SIGINT, handle_sigint);

    auto ts = std::chrono::system_clock::now();
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        auto now = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - ts) >= std::chrono::seconds(1)) {
            interval_1s();
            ts = now;
        }
    }
}

void Modules::interval_1s() {
    spdlog::info("{}", LOGHEAD);
}

} // namespace modules
} // namespace core

#undef LOGHEAD