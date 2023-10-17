#pragma once
#include "datas.h"

namespace core {
namespace api {
namespace market {
    
using MarketOperateResult = core::base::datas::MarketOperateResult;

class Market {
    public:
        virtual bool is_ready() = 0;

        virtual MarketOperateResult subscribe() = 0;
        virtual MarketOperateResult unsubscribe() = 0;
};            

} // namespace market
    
} // namespace api
    
} // namespace core