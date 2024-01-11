#pragma once
#include <iostream>
#include <vector>

#include <core/datas.hpp>
#include <core/message/message.hpp>


#define LOGHEAD "[Strategy::" + std::string(__func__) + "]"

namespace {
struct Self {
    core::message::message::CommandChannel *command_channel = nullptr;
    core::message::message::MarketChannel *market_channel = nullptr;
};
} // namespace 


class Strategy {
public:
    Strategy() : self( *new ::Self{} ) {}
    virtual ~Strategy() {
        delete &self;
    }

public:
    virtual std::string project_name() = 0;
    virtual core::datas::MessageType message_type() = 0;

    virtual void task() = 0;

    virtual void on_market_bbo(core::datas::Market_bbo* bbo) {}
    virtual void on_market_depth(core::datas::Market_depth* depth) {}
    virtual void on_market_kline(core::datas::Market_kline* kline) {}
    
    virtual void on_market() = 0;
    virtual void on_traded() = 0;
    virtual void on_order() = 0;

public:
    virtual bool subscribe(core::datas::SymbolObj symbols) final {
        core::datas::SymbolObj obj = std::move(symbols);
        obj.command_type = core::datas::CommandType::SUBSCRIBE;
        return self.command_channel->write(obj);
    }

    virtual bool unsubscribe(core::datas::SymbolObj symbols) final {
        core::datas::SymbolObj obj = std::move(symbols);
        obj.command_type = core::datas::CommandType::UNSUBSCRIBE;
        return self.command_channel->write(obj);
    }

    virtual bool order() final {
        return false;
    }

    virtual bool cancel() final {
        return false;
    }

    virtual void run() final {
        custom_init();

        std::thread task_thread(std::bind(&Strategy::task, this));
        task_thread.detach();

        uint64_t command_displacement = self.command_channel->earliest_displacement();
        uint64_t market_displacement = self.market_channel->earliest_displacement();
        while (true) {
            /*****************/
            /* read commands */
            /*****************/
            std::pair<core::datas::Command_base*, core::datas::CommandDataHeader*> command_pair = self.command_channel->read_next(command_displacement);
            if (command_pair.first) {
                switch (command_pair.first->command_type) {
                case core::datas::CommandType::SUBSCRIBE:
                    break;
                case core::datas::CommandType::UNSUBSCRIBE:
                    break;
                case core::datas::CommandType::ORDER:
                    break;
                case core::datas::CommandType::CANCEL:
                    break;
                default:
                    break;
                }
                continue;
            }

            /*****************/
            /** read market **/
            /*****************/
            std::pair<core::datas::Market_base*, core::datas::MarketDataHeader*> market_pair = self.market_channel->read_next(market_displacement);
            if (market_pair.first) {
                switch (market_pair.first->market_type) {
                case core::datas::MarketType::Bbo: {
                    core::datas::Market_bbo* obj = reinterpret_cast<core::datas::Market_bbo*>(market_pair.first);
                    if (obj) {
                        on_market_bbo(std::move(obj));
                    }
                    break;
                }
                case core::datas::MarketType::Depth: {
                    core::datas::Market_depth* obj = reinterpret_cast<core::datas::Market_depth*>(market_pair.first);
                    if (obj) {
                        on_market_depth(std::move(obj));
                    }
                    break;
                }
                case core::datas::MarketType::Kline: {
                    core::datas::Market_kline* obj = reinterpret_cast<core::datas::Market_kline*>(market_pair.first);
                    if (obj) {
                        on_market_kline(std::move(obj));
                    }
                    break;
                }
                default:
                    break;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

private:
    void custom_init() {
        std::string proj = project_name();
        core::datas::MessageType type = message_type();

        switch (type) {
        case core::datas::MessageType::ShareMemory: {
            self.command_channel = new core::message::message::CommandChannel(proj, 40 * MB);
            self.market_channel = new core::message::message::MarketChannel(proj, 40 * MB);

            assert(self.command_channel != nullptr);
            assert(self.market_channel != nullptr);
            break;
        }
        default:
            assert(false && "unsupport message type!");
            break;
        }
    }

private:
    Self &self;
};

#undef LOGHEAD