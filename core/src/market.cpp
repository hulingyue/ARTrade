#include "core/market.h"


namespace {
struct Self {
    core::message::message::MarketChannel *channel = nullptr;
};
}


namespace core::api::market {

Market::Market() : self( *new Self{} ) {

}

Market::~Market() {
    delete &self;
}

void Market::interval_1s() {

}

void Market::set_channel(core::message::message::MarketChannel *channel) {
    if (self.channel) {
        delete self.channel;
    }
    self.channel = channel;
}

/*****************************/
/** event                   **/
/*****************************/
void Market::on_market(core::datas::Market_bbo &obj) {
    if (self.channel == nullptr) {
        return;
    }

    self.channel->write(obj);
}

} // core::api::market