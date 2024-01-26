#pragma once
#include <vector>
#include "datas.hpp"
#include "pimpl.h"
#include "core/message/message.hpp"
#include "modules.h"


// forward declaration
namespace core::message { class Message; }
namespace core::modules { class Modules; }


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

        virtual void instruments() = 0;

        virtual void set_channel(core::message::message::MarketChannel *channel) final;
        virtual void set_modules(core::modules::Modules* modules) final;
        virtual core::modules::Modules* modules() final;

        // event
        void on_market(core::datas::Market_bbo &obj);
    private:
        Self &self;
};         

} // namespace core::api::market