#include "modules.h"
#include "pimpl.h"

#include "bybit_market.h"

class Bybit final : public core::modules::Modules {
public:
    Bybit();
    ~Bybit();

    void init_config() override;

    void interval_1s() override;

    BybitMarket& market();

private:
    Self &self;
};