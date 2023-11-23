#include "bybit_trade.h"


#define LOGHEAD "[BybitTrade::" std::string(__func__) "]"

namespace {
struct Self {
    std::atomic<bool> is_ready;
};
}


BybitTrade::BybitTrade() : self ( *new Self {} ) {
    self.is_ready.store(false);
}

BybitTrade::~BybitTrade() {
    delete &self;
}

void BybitTrade::init() {

}

bool BybitTrade::is_ready() {
    return self.is_ready.load();
}

TradeOperateResult BybitTrade::order(core::datas::OrderObj const &order) {

}

TradeOperateResult BybitTrade::cancel(core::datas::CancelObj const &order) {

}

void BybitTrade::interval_1s() {

}

void BybitTrade::on_open() {
    self.is_ready.store(true);
}

void BybitTrade::on_close() {
    self.is_ready.store(false);
}

void BybitTrade::on_message(std::string const &msg) {

}

void BybitTrade::ping() {

}

#undef LOGHEAD