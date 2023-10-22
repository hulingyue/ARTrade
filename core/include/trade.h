#pragma once 
#include "datas.h"

namespace core {
namespace api {
namespace trade {
    
using TradeOperateResult = core::base::datas::TradeOperateResult;

class Trade {
    public:
        Trade() = default;
        virtual ~Trade() = default;

        virtual bool is_ready() = 0;

        virtual TradeOperateResult order() = 0;
        virtual TradeOperateResult cancel() = 0;
};            

} // namespace trade
    
} // namespace api
    
} // namespace core