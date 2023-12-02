#pragma once 
#include "datas.hpp"
#include "pimpl.h"


// forward declaration
namespace core::message {
    class Message;
}


namespace core::api::trade {
    
using TradeOperateResult = core::datas::TradeOperateResult;

class Trade {
    public:
        Trade();
        virtual ~Trade();

        virtual void init() = 0;
        virtual bool is_ready() = 0;

        virtual TradeOperateResult order(core::datas::OrderObj const &order) = 0;
        virtual TradeOperateResult cancel(core::datas::CancelObj const &order) = 0;

        virtual void set_message(core::message::Message* messsage) final;
    
        // event
    private:
        Self &self;
};            

} // namespace core::api::trade
