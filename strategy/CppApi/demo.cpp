#include <iostream>
#include "strategy.hpp"


class Strategy_Demo : public Strategy {
public:

    Strategy_Demo() {
        project_name("Bybit");
        message_type(core::datas::MessageType::ShareMemory);

        set_task(std::bind(&Strategy_Demo::task, this));
        set_on_market_bbo(std::bind(&Strategy_Demo::on_market_bbo, this, std::placeholders::_1));
    }

    void task() {
        core::datas::SymbolObj symbol_obj;
        symbol_obj.command_type = core::datas::CommandType::UNKNOW;
        symbol_obj.push_back("tickers.BTCUSDT");
        symbol_obj.push_back("tickers.ETHUSDT");

        subscribe(symbol_obj);
        while (true) {

        }
    }

    void on_market_bbo(core::datas::Market_bbo* bbo) {
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