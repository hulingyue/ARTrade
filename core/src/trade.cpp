#include "core/trade.h"


namespace {
struct Self {
    core::message::Message* message = nullptr;
    core::modules::Modules* modules = nullptr;
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

void Trade::set_modules(core::modules::Modules* modules) { self.modules = modules; }
core::modules::Modules* Trade::modules() { return self.modules; }

/*****************************/
/** event                   **/
/*****************************/

} // namespace core::api::trade