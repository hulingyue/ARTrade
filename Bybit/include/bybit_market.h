#pragma once
#include "ws_client.h"
#include "market.h"
#include "datas.h"
#include "pimpl.h"

using MarketOperateResult = core::base::datas::MarketOperateResult;


class BybitMarket final : public core::api::market::Market {
public:
    BybitMarket();
    ~BybitMarket();

    bool is_ready() override;

    MarketOperateResult subscribe(const std::vector<std::string> symbols) override;
    MarketOperateResult unsubscribe(const std::vector<std::string> symbols) override;

    void interval_1s();

private:
    void on_open();
    void on_close();
    void on_message(std::string const &msg);

    void ping();

private:
    Self &self;
};