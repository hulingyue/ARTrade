#pragma once
#include "datas.hpp"

namespace core {
namespace api {
namespace market {
    
using MarketOperateResult = core::base::datas::MarketOperateResult;

class Market {
    public:
        Market() = default;
        virtual ~Market() = default;

        virtual bool is_ready() = 0;
        virtual void interval_1s() {}

        virtual MarketOperateResult subscribe(const std::vector<std::string> symbols) = 0;
        virtual MarketOperateResult unsubscribe(const std::vector<std::string> symbols) = 0;
};            

} // namespace market
    
} // namespace api
    
} // namespace core