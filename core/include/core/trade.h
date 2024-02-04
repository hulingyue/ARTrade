#pragma once 
#include "datas.hpp"
#include "pimpl.h"
#include "modules.h"


// forward declaration
namespace core::message { class Message; }
namespace core::modules { class Modules; }


namespace core::api::trade {
    
using TradeOperateResult = core::datas::TradeOperateResult;

class Trade {
    public:
        Trade();
        virtual ~Trade();

        virtual void init() = 0;
        virtual bool is_ready() = 0;

        virtual TradeOperateResult order(core::datas::OrderObj &order) = 0;
        virtual TradeOperateResult cancel(core::datas::CancelObj &order) = 0;

        virtual void set_message(core::message::Message* messsage) final;
        virtual void set_modules(core::modules::Modules* modules) final;
        virtual core::modules::Modules* modules() final;
    
        // event
    private:
        Self &self;
};            

} // namespace core::api::trade
