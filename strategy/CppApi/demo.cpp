#include <iostream>
#include "strategy.hpp"


class Strategy_Demo : public Strategy {
public:
    virtual std::string project_name() override {
        return "Bybit";
    }

    virtual core::datas::MessageType message_type() override {
        return core::datas::MessageType::ShareMemory;
    }

    virtual void task() override {
        core::datas::SymbolObj symbol_obj;
        symbol_obj.command_type = core::datas::CommandType::UNKNOW;
        symbol_obj.push_back("tickers.BTCUSDT");
        symbol_obj.push_back("tickers.ETHUSDT");

        subscribe(symbol_obj);
        while (true) {

        }
        
    }
    
    virtual void on_market() override {
    
    }
    
    virtual void on_traded() override {
    
    }
    
    virtual void on_order() override {
    
    }

    virtual void on_market_bbo(core::datas::Market_bbo* bbo) {
        if (bbo) {
            std::cout << bbo->exchange << " " << bbo->symbol << " " << bbo->time << " " << bbo->price << " " << bbo->quantity << std::endl;
        } else {
            std::cout << "bbo is nullptr" << std::endl;
        }
    }

};


int main(int argc, char** argv) {
    Strategy_Demo demo;
    demo.run();
    return 0;
}