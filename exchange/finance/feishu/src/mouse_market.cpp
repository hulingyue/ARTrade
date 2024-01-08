#include "mouse_market.h"


namespace {
struct Self {

};
}

MouseMarket::MouseMarket() : self( *new Self{}) {}

MouseMarket::~MouseMarket() {}

void MouseMarket::init() {}

bool MouseMarket::is_ready() {}

MarketOperateResult MouseMarket::subscribe(const std::vector<std::string> symbols) {}

MarketOperateResult MouseMarket::unsubscribe(const std::vector<std::string> symbols) {}

void MouseMarket::interval_1s() {}