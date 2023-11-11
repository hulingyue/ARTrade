#pragma once
#include <iostream>
#include <csignal>

#include <nlohmann/json.hpp>
#include "pimpl.h"
#include "trade.h"
#include "market.h"


namespace core::modules {

class Modules {
public:
    Modules();
    virtual ~Modules();

    // Market
    virtual void set_market_obj(core::api::market::Market* market_obj = nullptr);
    virtual bool is_market_ready();

    // Trade
    virtual void set_trade_obj(core::api::trade::Trade* trade_obj = nullptr);
    virtual bool is_trade_ready();

    // Config
    virtual void init_config();

    // 
    virtual bool is_ready();
    virtual void run();
    virtual void interval_1s();

private:
    // Config
    void default_config();
private:
    Self &self;
};

} // namespace core::modules