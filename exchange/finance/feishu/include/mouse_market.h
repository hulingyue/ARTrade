#pragma once
#include <core/ws_client.h>
#include <core/market.h>
#include <core/datas.hpp>
#include <core/pimpl.h>

#include <SgitFtdcMdApi.h>

using MarketOperateResult = core::datas::MarketOperateResult;
using namespace fstech;


class MouseMarket final : public core::api::market::Market, public CThostFtdcMdSpi {
public:
    MouseMarket();
    ~MouseMarket();

    void init() override;

    bool is_ready() override;

    MarketOperateResult subscribe(const std::vector<std::string> symbols) override;
    MarketOperateResult unsubscribe(const std::vector<std::string> symbols) override;

    void interval_1s();

private:
    Self &self;
};