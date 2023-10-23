#include <iostream>
#include <ctime>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "binance.h"

using namespace std::chrono_literals;

int main() {
    Binance binance;
    do {
        spdlog::info("binance is market ready? {}", binance.is_market_ready() ? " true" : "false");
        std::this_thread::sleep_for(1000ms);
    } while (!binance.is_market_ready());

    std::this_thread::sleep_for(1000ms);
    binance.market().subscribe();
    binance.run();
    return 0;
}