#pragma once
#include <vector>
#include <unordered_map>


namespace {
class SingleOrderBook {

};
}

namespace core::orderbook {
class OrderBook {

private:
    SingleOrderBook asks;
    SingleOrderBook bids;
};
}