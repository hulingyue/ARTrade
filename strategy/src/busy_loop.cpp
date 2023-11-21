#include "busy_loop.h"
#include <core/message.hpp>


#define LOGHEAD "[Strategy::" + std::string(__func__) + "]"

namespace {
struct Self {
    core::message::Message* message = nullptr;
};

using namespace core::datas;
using namespace strategy::datas;


static inline bool market(core::message::Message* message, std::vector<std::string> symbols, bool is_subscribe) {
    std::vector<std::string> container;
    bool status = true;

    auto market_event = [&](std::vector<std::string> symbols, bool is_subscribe) {
        CommandObj obj;
        obj.type = is_subscribe ? CommandType::SUBSCRIBE : CommandType::UNSUBSCRIBE;
        for (std::size_t index = 0; index < symbols.size(); index++) {
            std::strncpy(obj.symbols.symbols[index], symbols[index].c_str(), core::datas::SYMBOL_MAX_LENGTH);
            // obj.symbols.symbols[index][15] = '\0';
        }

        status &= message->write_command(obj);
    };

    for (std::size_t count = 0; count < symbols.size(); count++) {

        if (container.size() >= core::datas::SUBSCRIBE_MAX_SIZE) {
            market_event(container, is_subscribe);
        }

        container.emplace_back(std::move(symbols[count]));
    }

    if (container.size() > 0) {
        market_event(container, is_subscribe);
    }

    return status;
}
} // namespace 


Strategy::Strategy() : self( *new Self{} ){
}

Strategy::~Strategy() {
    delete &self;
}

bool Strategy::subscribe(std::vector<std::string> symbols) {
    if (!self.message) { return false; }
    return market(self.message, std::move(symbols), true);
}

bool Strategy::unsubscribe(std::vector<std::string> symbols) {
    if (!self.message) { return false; }
    return market(self.message, std::move(symbols), false);
}

bool Strategy::order() {
    return false;
}

bool Strategy::cancel() {
    return false;
}

void Strategy::run() {
    custom_init();

    std::thread task_thread(std::bind(&Strategy::task, this));
    task_thread.detach();

    while (true) {
        // read commands
        core::datas::CommandObj* command_obj = self.message->read_command();
        if (command_obj) {
            continue;
        }

        // read market
        core::datas::MarketObj* market_obj = self.message->read_market();
        if (market_obj) {
            if (market_obj->market_type == MarketType::Bbo) {
                MarketResponseBbo bbo;
                std::memcpy(bbo.exchange, market_obj->exchange, strategy::datas::SYMBOL_MAX_LENGTH);
                std::memcpy(bbo.symbol, market_obj->symbol, strategy::datas::SYMBOL_MAX_LENGTH);
                bbo.time = market_obj->time;

                bbo.price = market_obj->newest.price;
                bbo.quantity = market_obj->newest.quantity;
                on_market_bbo(&bbo);
            } else if (market_obj->market_type == MarketType::Depth) {
                MarketResponseDepth depth;
                std::memcpy(depth.exchange, market_obj->exchange, strategy::datas::SYMBOL_MAX_LENGTH);
                std::memcpy(depth.symbol, market_obj->symbol, strategy::datas::SYMBOL_MAX_LENGTH);
                depth.time = market_obj->time;

                std::memcpy(depth.asks, market_obj->asks, sizeof(strategy::datas::TradePair) * strategy::datas::MARKET_MAX_DEPTH);
                std::memcpy(depth.bids, market_obj->bids, sizeof(strategy::datas::TradePair) * strategy::datas::MARKET_MAX_DEPTH);
                on_market_depth(&depth);
            } else if (market_obj->market_type == MarketType::Kline) {
                MarketResponseKline kline;
                std::memcpy(kline.exchange, market_obj->exchange, strategy::datas::SYMBOL_MAX_LENGTH);
                std::memcpy(kline.symbol, market_obj->symbol, strategy::datas::SYMBOL_MAX_LENGTH);
                kline.time = market_obj->time;

                kline.high = market_obj->high;
                kline.low = market_obj->low;
                kline.open = market_obj->open;
                kline.close = market_obj->close;
                on_market_kline(&kline);
            }
            continue;
        }

        // 
    }
}

void Strategy::custom_init() {
    std::string proj = project_name();
   MessageType type = message_type();
    self.message = new core::message::Message(proj, type,Identity::Slave);

}

void Strategy::on_market_bbo(strategy::datas::MarketResponseBbo* bbo) {

}

void Strategy::on_market_depth(strategy::datas::MarketResponseDepth* depth) {

}

void Strategy::on_market_kline(strategy::datas::MarketResponseKline* kline) {

}

#undef LOGHEAD