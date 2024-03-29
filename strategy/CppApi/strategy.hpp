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
    void set_on_traded(std::function<void(core::datas::OrderObj*)> func) { on_traded = func; }
    void set_on_order(std::function<void(core::datas::OrderObj*)> func) { on_order = func; }

public:
    virtual bool subscribe(core::datas::SymbolObj& symbols) final {
        symbols.command_type = core::datas::CommandType::SUBSCRIBE;
        return command_channel->write(symbols);
    }

    virtual bool unsubscribe(core::datas::SymbolObj& symbols) final {
        symbols.command_type = core::datas::CommandType::UNSUBSCRIBE;
        return command_channel->write(symbols);
    }

    virtual bool order(core::datas::OrderObj &obj) final {
        return command_channel->write(obj);
    }

    virtual bool cancel(core::datas::CancelObj &obj) final {
        return command_channel->write(obj);
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
                if (command_pair.second && command_pair.second->status == core::datas::CommandStatus::INVALID) { continue; }
                switch (command_pair.first->command_type) {
                case core::datas::CommandType::SUBSCRIBE:{
                    core::datas::SymbolObj* obj = reinterpret_cast<core::datas::SymbolObj*>(command_pair.first);
                    break;
                }
                case core::datas::CommandType::UNSUBSCRIBE:{
                    break;
                }
                case core::datas::CommandType::ORDER:{
                    core::datas::OrderObj* obj = reinterpret_cast<core::datas::OrderObj*>(command_pair.first);
                    if (!obj) { break; }
                    // Todu: 这是个愚蠢的设计，共享内存增加消息
                    if (!(obj->status == core::datas::OrderStatus::REJECTED
                    || obj->status == core::datas::OrderStatus::FILLED
                    || obj->status == core::datas::OrderStatus::CANCEL
                    )) {
                        _exists_orders[obj] = *obj;
                    }
                    if (on_order) { on_order(obj); }
                    break;
                }
                case core::datas::CommandType::CANCEL:{
                    core::datas::CancelObj* obj = reinterpret_cast<core::datas::CancelObj*>(command_pair.first);
                    break;
                }
                default:
                    break;
                }
                continue;
            }

            // check exists command status
            // Todu: 这是个愚蠢的设计，共享内存增加消息
            for (auto it = _exists_orders.begin(); it != _exists_orders.end();) {
                if (*(it->first) == it->second) { continue; }
                it->second = *(it->first);
                if (on_order) { on_order(&it->second); }

                if (it->second.status == core::datas::OrderStatus::REJECTED
                || it->second.status == core::datas::OrderStatus::FILLED
                || it->second.status == core::datas::OrderStatus::CANCEL
                ) {
                    it = _exists_orders.erase(it);
                } else {
                    it++;
                }
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
                        on_market_bbo(obj);
                    }
                    break;
                }
                case core::datas::MarketType::Depth: {
                    core::datas::Market_depth* obj = reinterpret_cast<core::datas::Market_depth*>(market_pair.first);
                    if (obj && on_market_depth) {
                        on_market_depth(obj);
                    }
                    break;
                }
                case core::datas::MarketType::Kline: {
                    core::datas::Market_kline* obj = reinterpret_cast<core::datas::Market_kline*>(market_pair.first);
                    if (obj && on_market_kline) {
                        on_market_kline(obj);
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

    std::unordered_map<core::datas::OrderObj*, core::datas::OrderObj> _exists_orders;

private:
    std::function<void()> task;

    std::function<void(core::datas::Market_bbo*)> on_market_bbo;
    std::function<void(core::datas::Market_depth*)> on_market_depth;
    std::function<void(core::datas::Market_kline*)> on_market_kline;

    std::function<void()> on_market;
    std::function<void(core::datas::OrderObj*)> on_traded;
    std::function<void(core::datas::OrderObj*)> on_order;
};

#undef LOGHEAD