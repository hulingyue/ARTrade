#include <iostream>
#include <ctime>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <core/util.hpp>
#include "bybit.h"

using namespace std::chrono_literals;

int main() {
    Bybit bybit;
    do {
        spdlog::info("bybit is market ready? {}", bybit.is_market_ready() ? " true" : "false");
        std::this_thread::sleep_for(1000ms);
    } while (!bybit.is_market_ready());

    std::this_thread::sleep_for(1000ms);
    bybit.market().subscribe({"tickers.BTCUSDT"});
    bybit.run();
    return 0;
}