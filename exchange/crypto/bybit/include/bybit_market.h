#pragma once
#include <core/ws_client.h>
#include <core/market.h>
#include <core/datas.hpp>
#include <core/pimpl.h>

using MarketOperateResult = core::datas::MarketOperateResult;


class BybitMarket final : public core::api::market::Market {
public:
    BybitMarket();
    ~BybitMarket();

    void init() override;

    bool is_ready() override;

    MarketOperateResult subscribe(const std::vector<std::string> symbols) override;
    MarketOperateResult unsubscribe(const std::vector<std::string> symbols) override;

    void interval_1s();

private:
    void instruments();

    void on_open();
    void on_close();
    void on_message(std::string const &msg);

    void ping();

private:
    Self &self;
};