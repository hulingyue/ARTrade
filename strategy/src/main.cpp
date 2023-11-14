#include "busy_loop.h"


class Strategy_Demo : public Strategy {
public:
    virtual std::string project_name() override {
        return "Bybit";
    }

    virtual core::base::datas::MessageType message_type() override {
        return core::base::datas::MessageType::ShareMemory;
    }

    virtual void task() override {
        std::vector<std::string> symbols {
            "BTCUSDT"
        };

        subscribe(std::move(symbols));
        while (true) {

        }
        
    }
    
    virtual void on_market() override {
    
    }
    
    virtual void on_traded() override {
    
    }
    
    virtual void on_order() override {
    
    }


};


int main(int argc, char** argv) {
    Strategy_Demo demo;
    demo.run();
    return 0;
}