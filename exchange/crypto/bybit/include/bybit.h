#include <core/modules.h>
#include <core/pimpl.h>
#include <core/datas.hpp>

#include "bybit_market.h"
#include "bybit_trade.h"

class Bybit final : public core::modules::Modules {
public:
    Bybit(int argc, char** argv);
    ~Bybit();
    
    std::string project_name() override;
    core::datas::MessageType message_type() override;

    void init_config() override;

    void interval_1s() override;

    BybitMarket& market();
    BybitTrade& trade();

private:
    Self &self;
};