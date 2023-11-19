#include "core/market.h"
#include "core/message.hpp"


namespace {
struct Self {
    core::message::Message *message;
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

void Market::set_message(core::message::Message* message) {
    self.message = message;
}

/*****************************/
/** event                   **/
/*****************************/
void Market::on_market(core::base::datas::MarketObj obj) {
    if (self.message == nullptr) {
        return;
    }

    self.message->write_market(std::move(obj));
}

} // core::api::market