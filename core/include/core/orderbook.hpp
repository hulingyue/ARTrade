#pragma once
#include <cassert>
#include <unordered_map>
#include "datas.hpp"

namespace core::datas { struct TradePair; }

namespace {
enum class PriceOrder {
    ESC,  // ask
    DESC  // bid
};

class SingleOrderBook {
public:
    SingleOrderBook(int depth, double accuracy, PriceOrder order)
        : _depth(depth), _accuracy(accuracy), _order(order) {
        data = new core::datas::TradePair[depth];
    }

    ~SingleOrderBook() {
        delete[] data;
    }

    void init(core::datas::TradePair pair[], int size) {
        map_index.clear();

        for (int index = 0; index < std::min(size, _depth); index++) {
            data[index] = pair[index];
            map_index[pair[index].price] = index;
        }

        for (int index = std::min(size, _depth); index < _depth; index++) {
            data[index] = {_accuracy, _accuracy};
        }
    }

    void update(core::datas::TradePair pair) {
        auto it = map_index.find(pair.price);
        if (it != map_index.end()) {
            data[map_index[pair.price]] = pair;
            return;
        }

        // find & move
        if (_order == PriceOrder::ESC) {
            for (int index = 0; index < _depth; index++) {
                if (data[index].price <= pair.price) {
                    std::copy(data + index, data + _depth - 1, data + index + 1);
                    data[index] = pair;
                    break;
                }
            }
        } else if (_order == PriceOrder::DESC) {
            for (int index = 0; index < _depth; index++) {
                if (data[index].price >= pair.price) {
                    std::copy(data + index, data + _depth - 1, data + index + 1);
                    data[index] = pair;
                    break;
                }
            }
        } else {
            assert(false && "unknown PriceOrder!");
        }
    }

    void update(double price, double quantity) {
        update({.price = price, .quantity = quantity});
    }

    core::datas::TradePair* get() {
        return data;
    }

private:
    int _depth;
    double _accuracy;
    PriceOrder _order;

    // price -> location
    std::unordered_map<double, int> map_index;
    core::datas::TradePair *data;
};
} // namespace

namespace core::orderbook {
class OrderBook {
public:
    OrderBook(int depth, double accuracy=0.00001) {
        asks = new SingleOrderBook(depth, accuracy, PriceOrder::ESC);
        bids = new SingleOrderBook(depth, accuracy, PriceOrder::DESC);
    }

    ~OrderBook() {
        delete asks;
        delete bids;
    }

    void init_asks(core::datas::TradePair pair[], int size) { asks->init(pair, size); }
    void init_bids(core::datas::TradePair pair[], int size) { bids->init(pair, size); }

    void update_asks(double price, double quantity) { asks->update(price, quantity); }
    void update_bids(double price, double quantity) { bids->update(price, quantity); }

    core::datas::TradePair* get_asks() { return asks->get(); }
    core::datas::TradePair* get_bids() { return bids->get(); }
    std::pair<core::datas::TradePair*, core::datas::TradePair*> get() {
        return std::make_pair(asks->get(), bids->get());
    }

private:
    SingleOrderBook *asks;
    SingleOrderBook *bids;
};
} // namespace core::orderbook