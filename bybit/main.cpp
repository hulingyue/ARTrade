#include <iostream>
#include <ctime>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <core/util.h>
#include <core/message.hpp>
#include "bybit.h"

using namespace std::chrono_literals;
using namespace core::message;

int main(int argc, char **argv) {
    Bybit bybit;

    core::util::startup(&bybit, argc, argv);

    do {
        spdlog::info("bybit is market ready? {}", bybit.is_market_ready() ? " true" : "false");
        std::this_thread::sleep_for(1000ms);
    } while (!bybit.is_market_ready());

    std::this_thread::sleep_for(1000ms);
    bybit.market().subscribe({"tickers.BTCUSDT"});
    bybit.run();
    return 0;
}