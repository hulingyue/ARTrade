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

        virtual bool is_ready() = 0;

        virtual TradeOperateResult order() = 0;
        virtual TradeOperateResult cancel() = 0;

        virtual void set_message(core::message::Message* messsage) final;
    
        // event
    private:
        Self &self;
};            

} // namespace core::api::trade
