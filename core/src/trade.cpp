#include "core/trade.h"


namespace {
struct Self {
    core::message::Message* message;
};
}


namespace core::api::trade {

Trade::Trade() : self ( *new Self{} ) {

}

Trade::~Trade() {
    delete &self;
}

void Trade::set_message(core::message::Message* message) {
    self.message = message;
}

/*****************************/
/** event                   **/
/*****************************/

} // namespace core::api::trade