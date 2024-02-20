#pragma once
#include <cassert>
#include <unordered_map>
#include "datas.hpp"

namespace core::datas { struct TradePair; }

namespace {
constexpr int MULTIPLE = 1000000;
constexpr double ACCURACY = 0.000001;

enum class PriceOrder {
    ESC,  // ask
    DESC  // bid
};

template<PriceOrder order, int depth>
class SingleOrderBook {
public:
    SingleOrderBook() {
        _prices = new core::datas::TradePair[depth];
        std::memset(_prices, 0, depth * sizeof(core::datas::TradePair));
    }

    ~SingleOrderBook() {
        delete []_prices;
    }

    double best_price() noexcept {
        return static_cast<double>((_prices[0].price * 1.0) / MULTIPLE);
    }

    core::datas::TradePair best_pair() noexcept {
        return _prices[0];
    }

    core::datas::TradePair* get() noexcept {
        return _prices;
    }

    void init(core::datas::TradePair pairs[], int size) {
        // init
        map.clear();
        std::memset(_prices, 0, depth * sizeof(core::datas::TradePair));
    
        for (int index = 0; index < std::min(size, depth); index++) {
            long price = static_cast<long>(pairs[index].price * MULTIPLE);
            _prices[index].price = price;
            _prices[index].quantity = pairs[index].quantity;
            map[price] = index;
        }
    }

    void update(core::datas::TradePair pair) {
        long price = static_cast<long>(pair.price * MULTIPLE);
        auto it = map.find(price);

        // exist
        if (it != map.end()) {
            _prices[it->second].quantity = pair.quantity;
            return;
        }

        if (pair.quantity > ACCURACY) {
            push_order(std::move(pair));
        } else if (pair.quantity < ACCURACY) {
            pop_order(std::move(pair));
        } else {

        }
    }

    void push_order(core::datas::TradePair pair) {
        long price = static_cast<long>(pair.price * MULTIPLE);
        auto it = map.find(price);

        // exist
        if (it != map.end()) {
            it->second += pair.quantity;
            return;
        }

        // not exist
        for (int index = 0; index < depth; index++) {
            if ((order == PriceOrder::ESC && price < _prices[index].price) || (order == PriceOrder::DESC && price > _prices[index].price)) {
                memcpy(_prices + index + 1, _prices + index, (depth - index - 1) * sizeof(core::datas::TradePair));
                _prices[index].price = price;
                _prices[index].quantity = pair.quantity;
                map[price] = index;
                return;
            }
        }
    }

    void pop_order(core::datas::TradePair pair) {
        long price = static_cast<long>(pair.price * MULTIPLE);
        auto it = map.find(price);

        // not exist
        if (it == map.end()) {
            return;
        }

        // exist
        it->second -= pair.quantity;
        if (it->second > ACCURACY) {
            return;
        }

        map.erase(it);
        for (int index = 0; index < depth; index++) {
            if (price == _prices[index].price) {
                memcpy(_prices + index, _prices + index + 1, (depth - index - 1) * sizeof(core::datas::TradePair));
                if (order == PriceOrder::ESC) {
                    _prices[depth - 1].price = INTMAX_MAX;
                } else if (order == PriceOrder::DESC) {
                    _prices[depth - 1].price = INTMAX_MIN;
                }
                return;
            }
        }
    }

private:
    // key: price value: location
    std::unordered_map<long, int> map;
    // price queue
    core::datas::TradePair* _prices;
};
} // namespace

namespace core::orderbook {
class OrderBook {
public:
    double best_ask() noexcept { return _ask.best_price(); }
    double best_bid() noexcept { return _bid.best_price(); }

    core::datas::TradePair best_ask_pair() noexcept { return _ask.best_pair(); }
    core::datas::TradePair best_bid_pair() noexcept { return _bid.best_pair(); }

    core::datas::TradePair* get_asks() noexcept { return _ask.get(); }
    core::datas::TradePair* get_bids() noexcept { return _bid.get(); }

    void init_ask(core::datas::TradePair pairs[], int size) { return _ask.init(pairs, size); }
    void init_bid(core::datas::TradePair pairs[], int size) { return _bid.init(pairs, size); }

    void update_ask(core::datas::TradePair pair) { return _ask.update(std::move(pair)); }
    void update_bid(core::datas::TradePair pair) { return _bid.update(std::move(pair)); }

    void push_ask_order(core::datas::TradePair pair) { return _ask.push_order(std::move(pair)); }
    void push_bid_order(core::datas::TradePair pair) { return _bid.push_order(std::move(pair)); }

    void pop_ask_order(core::datas::TradePair pair) { return _ask.pop_order(std::move(pair)); }
    void pop_bid_order(core::datas::TradePair pair) { return _bid.pop_order(std::move(pair)); }
private:
    SingleOrderBook<PriceOrder::ESC, core::datas::MARKET_MAX_DEPTH> _ask;
    SingleOrderBook<PriceOrder::DESC, core::datas::MARKET_MAX_DEPTH> _bid;
};

class OrderbookManager {
public:
    OrderbookManager() = default;
    ~OrderbookManager() = default;

    void init_asks(std::string symbol, core::datas::TradePair pair[], int size) {
        return _map[symbol]->init_ask(pair, size);
    }

    void init_bids(std::string symbol, core::datas::TradePair pair[], int size) {
        return _map[symbol]->init_bid(pair, size);
    }

    void update_asks(std::string symbol, int depth, double price, double quantity) {
        return _map[symbol]->update_ask({price, quantity});
    }

    void update_bids(std::string symbol, int depth, double price, double quantity) {
        return _map[symbol]->update_bid({price, quantity});
    }

    core::datas::TradePair* get_asks(std::string symbol, int depth) {
        return _map[symbol]->get_asks();
    }

    core::datas::TradePair* get_bids(std::string symbol, int depth) {
        return _map[symbol]->get_bids();
    }

    std::pair<core::datas::TradePair*, core::datas::TradePair*> get(std::string symbol, int depth) {
        return std::make_pair(get_asks(symbol, depth), get_bids(symbol, depth));
    }

    void key_exist_or_create(std::string symbol, int depth, double accuracy) {
        auto it = _map.find(symbol);
        if (it == _map.end()) {
            _map[symbol] = new OrderBook();
        }
    }

private:
    // symbol<depth, info>
    std::unordered_map<std::string, OrderBook*> _map;
};
} // namespace core::orderbook