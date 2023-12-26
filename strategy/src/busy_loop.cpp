#include "busy_loop.h"
#include <core/message/message.hpp>


#define LOGHEAD "[Strategy::" + std::string(__func__) + "]"

namespace {
struct Self {
    core::message::message::CommandChannel *command_channel = nullptr;
    core::message::message::MarketChannel *market_channel = nullptr;
};
} // namespace 


Strategy::Strategy() : self( *new Self{} ){
}

Strategy::~Strategy() {
    delete &self;
}

bool Strategy::subscribe(core::datas::SymbolObj symbols) {
    std::cout << "subscribe" << std::endl;
    core::datas::SymbolObj obj = std::move(symbols);
    obj.command_type = core::datas::CommandType::SUBSCRIBE;
    return self.command_channel->write(obj);
}

bool Strategy::unsubscribe(core::datas::SymbolObj symbols) {
    core::datas::SymbolObj obj = std::move(symbols);
    obj.command_type = core::datas::CommandType::UNSUBSCRIBE;
    return self.command_channel->write(obj);
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

        // core::datas::CommandObj* command_obj = self.message->read_command();
        // if (command_obj) {
        //     continue;
        // }

        // // read market
        // core::datas::MarketObj* market_obj = self.message->read_market();
        // if (market_obj) {
        //     if (market_obj->market_type == MarketType::Bbo) {
        //         MarketResponseBbo bbo;
        //         std::memcpy(bbo.exchange, market_obj->exchange, strategy::datas::SYMBOL_MAX_LENGTH);
        //         std::memcpy(bbo.symbol, market_obj->symbol, strategy::datas::SYMBOL_MAX_LENGTH);
        //         bbo.time = market_obj->time;

        //         bbo.price = market_obj->newest.price;
        //         bbo.quantity = market_obj->newest.quantity;
        //         on_market_bbo(&bbo);
        //     } else if (market_obj->market_type == MarketType::Depth) {
        //         MarketResponseDepth depth;
        //         std::memcpy(depth.exchange, market_obj->exchange, strategy::datas::SYMBOL_MAX_LENGTH);
        //         std::memcpy(depth.symbol, market_obj->symbol, strategy::datas::SYMBOL_MAX_LENGTH);
        //         depth.time = market_obj->time;

        //         std::memcpy(depth.asks, market_obj->asks, sizeof(strategy::datas::TradePair) * strategy::datas::MARKET_MAX_DEPTH);
        //         std::memcpy(depth.bids, market_obj->bids, sizeof(strategy::datas::TradePair) * strategy::datas::MARKET_MAX_DEPTH);
        //         on_market_depth(&depth);
        //     } else if (market_obj->market_type == MarketType::Kline) {
        //         MarketResponseKline kline;
        //         std::memcpy(kline.exchange, market_obj->exchange, strategy::datas::SYMBOL_MAX_LENGTH);
        //         std::memcpy(kline.symbol, market_obj->symbol, strategy::datas::SYMBOL_MAX_LENGTH);
        //         kline.time = market_obj->time;

        //         kline.high = market_obj->high;
        //         kline.low = market_obj->low;
        //         kline.open = market_obj->open;
        //         kline.close = market_obj->close;
        //         on_market_kline(&kline);
        //     }
        //     continue;
        // }

        // 
    }
}

void Strategy::custom_init() {
    std::string proj = project_name();
    core::datas::MessageType type = message_type();
    self.command_channel = new core::message::message::CommandChannel(proj, 40 * MB);
    self.market_channel = new core::message::message::MarketChannel(proj, 40 * KB);

    assert(self.command_channel != nullptr);
    assert(self.market_channel != nullptr);
}

void Strategy::on_market_bbo(core::datas::Market_bbo* bbo) {}

void Strategy::on_market_depth(core::datas::Market_depth* depth) {}

void Strategy::on_market_kline(core::datas::Market_kline* kline) {}

#undef LOGHEAD