#include <core/modules.h>
#include <core/pimpl.h>

#include "bybit_market.h"

class Bybit final : public core::modules::Modules {
public:
    Bybit(int argc, char** argv);
    ~Bybit();

    void init_config() override;

    void interval_1s() override;

    BybitMarket& market();

private:
    Self &self;
};