#include "binance_market.h"


BinanceMarket::BinanceMarket() {

}

BinanceMarket::~BinanceMarket() {

}

bool BinanceMarket::is_ready() {
    return true;
}

MarketOperateResult BinanceMarket::subscribe() {
    return {
        .code = 0,
        .msg = ""
    };
}

MarketOperateResult BinanceMarket::unsubscribe() {
    return {
        .code = 0,
        .msg = ""
    };
}