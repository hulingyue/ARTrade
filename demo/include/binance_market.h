#include "market.h"
#include "datas.h"

using MarketOperateResult = core::base::datas::MarketOperateResult;


class BinanceMarket final: core::api::market::Market {
public:
    BinanceMarket();
    ~BinanceMarket();

    bool is_ready() override;

    MarketOperateResult subscribe() override;
    MarketOperateResult unsubscribe() override;
};