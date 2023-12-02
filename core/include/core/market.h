#pragma once
#include <vector>
#include "datas.hpp"
#include "pimpl.h"


// forward declaration
namespace core::message {
    class Message;
}


namespace core::api::market {
    
using MarketOperateResult = core::datas::MarketOperateResult;

class Market {
    public:
        Market();
        virtual ~Market();

        virtual void init() = 0;

        virtual bool is_ready() = 0;
        virtual void interval_1s();

        virtual MarketOperateResult subscribe(const std::vector<std::string> symbols) = 0;
        virtual MarketOperateResult unsubscribe(const std::vector<std::string> symbols) = 0;

        virtual void set_message(core::message::Message* messsage) final;

        // event
        virtual void on_market(core::datas::MarketObj obj) final;
    private:
        Self &self;
};         

} // namespace core::api::market