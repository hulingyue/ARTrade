#include "modules.h"
#include "pimpl.h"

#include "binance_market.h"

class Binance final : public core::modules::Modules {
public:
    Binance();
    ~Binance();

    void init_config();

private:
    Self &self;
};