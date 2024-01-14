#pragma once
#include <iostream>
#include <vector>

#include <core/datas.hpp>
#include <core/message/message.hpp>


#define LOGHEAD "[Strategy::" + std::string(__func__) + "]"


class Strategy {
public:
    Strategy() {}
    virtual ~Strategy() {}

public:
    std::string project_name() { return _project_name; }
    void project_name(std::string name) { _project_name = name; }

    core::datas::MessageType message_type() { return _message_type; }
    void message_type(core::datas::MessageType type) { _message_type = type; }

    void set_task(std::function<void()> func) { task = func; }

    void set_on_market_bbo(std::function<void(core::datas::Market_bbo*)> func) { on_market_bbo = func; }
    void set_on_market_depth(std::function<void(core::datas::Market_depth*)> func) { on_market_depth = func; }
    void set_on_market_kline(std::function<void(core::datas::Market_kline*)> func) { on_market_kline = func; }
    
    void set_on_market(std::function<void()> func) { on_market = func; }
    void set_on_traded(std::function<void()> func) { on_traded = func; }
    void set_on_order(std::function<void()> func) { on_order = func; }

public:
    virtual bool subscribe(core::datas::SymbolObj symbols) final {
        core::datas::SymbolObj obj = std::move(symbols);
        obj.command_type = core::datas::CommandType::SUBSCRIBE;
        return command_channel->write(obj);
    }

    virtual bool unsubscribe(core::datas::SymbolObj symbols) final {
        core::datas::SymbolObj obj = std::move(symbols);
        obj.command_type = core::datas::CommandType::UNSUBSCRIBE;
        return command_channel->write(obj);
    }

    virtual bool order() final {
        return false;
    }

    virtual bool cancel() final {
        return false;
    }

    virtual void monitor() final {
        uint64_t command_displacement = command_channel->earliest_displacement();
        uint64_t market_displacement = market_channel->earliest_displacement();
        while (true) {
            /*****************/
            /* read commands */
            /*****************/
            std::pair<core::datas::Command_base*, core::datas::CommandDataHeader*> command_pair = command_channel->read_next(command_displacement);
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
            std::pair<core::datas::Market_base*, core::datas::MarketDataHeader*> market_pair = market_channel->read_next(market_displacement);
            if (market_pair.first) {
                switch (market_pair.first->market_type) {
                case core::datas::MarketType::Bbo: {
                    core::datas::Market_bbo* obj = reinterpret_cast<core::datas::Market_bbo*>(market_pair.first);
                    if (obj && on_market_bbo) {
                        on_market_bbo(std::move(obj));
                    }
                    break;
                }
                case core::datas::MarketType::Depth: {
                    core::datas::Market_depth* obj = reinterpret_cast<core::datas::Market_depth*>(market_pair.first);
                    if (obj && on_market_depth) {
                        on_market_depth(std::move(obj));
                    }
                    break;
                }
                case core::datas::MarketType::Kline: {
                    core::datas::Market_kline* obj = reinterpret_cast<core::datas::Market_kline*>(market_pair.first);
                    if (obj && on_market_kline) {
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

    virtual void run() final {
        custom_init();

        std::thread monitor_thread(std::bind(&Strategy::monitor, this));
        monitor_thread.detach();

        if (task) {
            task();
        }
    }

private:
    void custom_init() {
        std::string proj = project_name();
        core::datas::MessageType type = message_type();

        switch (type) {
        case core::datas::MessageType::ShareMemory: {
            command_channel = new core::message::message::CommandChannel(proj, 40 * MB);
            market_channel = new core::message::message::MarketChannel(proj, 40 * MB);

            assert(command_channel != nullptr);
            assert(market_channel != nullptr);
            break;
        }
        default:
            assert(false && "unsupport message type!");
            break;
        }
    }

private:
    core::message::message::CommandChannel *command_channel = nullptr;
    core::message::message::MarketChannel *market_channel = nullptr;

private:
    std::string _project_name;
    core::datas::MessageType _message_type;

private:
    std::function<void()> task;

    std::function<void(core::datas::Market_bbo*)> on_market_bbo;
    std::function<void(core::datas::Market_depth*)> on_market_depth;
    std::function<void(core::datas::Market_kline*)> on_market_kline;

    std::function<void()> on_market;
    std::function<void()> on_traded;
    std::function<void()> on_order;
};

#undef LOGHEAD