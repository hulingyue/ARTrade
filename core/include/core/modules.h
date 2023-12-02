#pragma once
#include <iostream>
#include <csignal>

#include <nlohmann/json.hpp>
#include "pimpl.h"
#include "trade.h"
#include "market.h"
#include "datas.hpp"


namespace core::modules {

class Modules {
public:
    Modules(int argc, char** argv);
    virtual ~Modules();
    virtual void init(int argc, char** argv, size_t log_size = 5 * 1024 * 1024, size_t log_files = 10);

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

public:
    virtual std::string project_name() = 0;
    virtual core::datas::MessageType message_type() = 0;

private:
    // Config
    void default_config();
    void custom_init();
private:
    Self &self;
};

} // namespace core::modules