#pragma once
#include <core/trade.h>
#include <core/datas.hpp>
#include <core/pimpl.h>

using TradeOperateResult = core::datas::TradeOperateResult;


class BybitTrade final : public core::api::trade::Trade {
public:
    BybitTrade();
    ~BybitTrade();

    void init() override;

    bool is_ready() override;

    TradeOperateResult order(core::datas::OrderObj &order) override;
    TradeOperateResult cancel(core::datas::CancelObj &order) override;

    void interval_1s();

private:
    void on_open();
    void on_close();
    void on_message(std::string const &msg);

    void ping();
    void subscribe_private_channel();

private:
    Self &self;
};