// #include "bybit.h"

// int main(int argc, char **argv) {
//     Bybit bybit(argc, argv);

//     bybit.run();
//     return 0;
// }

#include <core/message/message.hpp>

using namespace core::message::message;


int main(int argc, char **argv) {
    MarketChannel market_channel("bybit", 40 * 1024 * 1024, false);
    return 0;
}