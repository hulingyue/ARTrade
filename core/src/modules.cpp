#include <iostream>
#include <ctime>
#include <filesystem>

#include "core/modules.h"
#include "core/config.h"
#include "core/util.h"
#include "core/order.hpp"


#define LOGHEAD "[Modules::" + std::string(__func__) + "]"

namespace {

struct Self {
    core::api::market::Market *market = nullptr;
    core::api::trade::Trade *trade = nullptr;
    core::message::message::MarketChannel *market_channel = nullptr;
    core::message::message::CommandChannel *command_channel = nullptr;

    std::unordered_map<std::string, core::datas::Instruments*> map_instruments;

    ~Self() {
        if (market) { delete market; }
        if (trade) { delete trade; }
        if (market_channel) { delete market_channel; }
        if (command_channel) { delete command_channel; }
    }
};

}

namespace core::modules {

Modules::Modules(int argc, char** argv) : self { *new Self{} } {
    // default setting
    init(argc, argv);
}

Modules::~Modules() {
    if (&self) { delete &self; }
}

void Modules::init(int argc, char** argv, size_t log_size, size_t log_files) {
    core::util::startup(this, argc, argv, log_size, log_files);
}

// Market
void Modules::set_market_obj(core::api::market::Market *market_obj) {
    if (self.market) {
        delete self.market;
    }
    self.market = market_obj;
}

bool Modules::is_market_ready() {
    if (self.market) {
        return self.market->is_ready();
    }

    return false;
}

// Trade
void Modules::set_trade_obj(core::api::trade::Trade *trade_obj) {
    if (self.trade) {
        delete self.trade;
    }
    self.trade = trade_obj;
}

bool Modules::is_trade_ready() {
    if (self.trade) {
        return self.trade->is_ready();
    }
    return false;
}

// Config
void Modules::init_config() {
    default_config();
    spdlog::info("{} init config with default config!", LOGHEAD);
}

void Modules::default_config() {
    nlohmann::json default_config = nlohmann::json::parse(
        R"(
            {
                "project": "default",
                "version": 1.0
            }
        )"
    );

    core::config::Config::init(default_config);
}

// 
bool Modules::is_ready() {
    if (self.trade && self.market) {
        return self.trade->is_ready() && self.market->is_ready();
    } else if (self.trade) {
        return self.trade->is_ready();
    } else if (self.market) {
        return self.market->is_ready();
    } 
    return false;
}

void handle_sigint(int signal) {
    std::cout << std::endl;
    spdlog::info("{} Received Ctrl+C signal, program is about to exit!", LOGHEAD);
    
    exit(signal);
}

void Modules::custom_init() {
    // something init after custom modules init

    // config
    init_config();
    core::util::create_folder(core::util::config_path());
    bool read_config_status = core::config::Config::read(
        (core::util::config_path() / std::filesystem::path("app.json")).string()
    );

    if (read_config_status == false) {
        spdlog::error("{} cannot found 'app.json' at {}", LOGHEAD, core::util::config_path().string());
        exit(-3);
    }
    
    // core::datas::MessageType type = message_type();
    std::string proj = project_name();
    self.market_channel = new core::message::message::MarketChannel(proj, 40 * MB, true);
    if (self.market) {
        self.market->set_channel(self.market_channel);
        self.market->init();
        self.market->set_modules(this);
        self.market->instruments();
        save_instruments();
    }
    self.command_channel = new core::message::message::CommandChannel(proj, 40 * MB);
    if (self.trade) {
        // self.trade->set_message(self.command_channel);
        self.trade->init();
        self.trade->set_modules(this);
    }
}


void Modules::run() {
    std::signal(SIGINT, handle_sigint);

    custom_init();

    // check if it is ready
    do {
        spdlog::info("modules is ready? {}", is_ready() ? "true" : "false");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (!is_ready());

    auto ts = std::chrono::system_clock::now();

    uint64_t address_displacement = self.command_channel->earliest_displacement();
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        auto now = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - ts) >= std::chrono::seconds(1)) {
            interval_1s();
            ts = now;
        }

        // read commands & deal with commands
        std::pair<core::datas::Command_base*, core::datas::CommandDataHeader*> command_pair = self.command_channel->read_next(address_displacement);
        if (command_pair.first && command_pair.second && command_pair.second->status == core::datas::CommandStatus::EFFECTIVE) {
            switch (command_pair.first->command_type) {
            case core::datas::CommandType::SUBSCRIBE:{
                core::datas::SymbolObj *obj = reinterpret_cast<core::datas::SymbolObj*>(command_pair.first);
                uint64_t size = obj->size();

                std::vector<std::string> symbols;
                for (uint64_t index = 0; index < size; index++) {
                    symbols.emplace_back(std::move(std::string(obj->symbols[index])));
                }

                core::datas::MarketOperateResult result = self.market->subscribe(std::vector<std::string>(std::move(symbols)));
                if (result.code == 0) {
                    command_pair.second->status = core::datas::CommandStatus::INVALID;
                }
                break;
            }
            case core::datas::CommandType::UNSUBSCRIBE: {
                core::datas::SymbolObj *obj = reinterpret_cast<core::datas::SymbolObj*>(command_pair.first);
                uint64_t size = obj->size();

                std::vector<std::string> symbols;
                for (uint64_t index = 0; index < size; index++) {
                    symbols.emplace_back(std::move(std::string(obj->symbols[index])));
                }

                core::datas::MarketOperateResult result = self.market->unsubscribe(std::vector<std::string>(std::move(symbols)));
                if (result.code == 0) {
                    command_pair.second->status = core::datas::CommandStatus::INVALID;
                }
                break;
            }
            case core::datas::CommandType::ORDER: {
                core::datas::OrderObj *obj = reinterpret_cast<core::datas::OrderObj*>(command_pair.first);
                if (!obj) {
                    spdlog::error("{} ORDER cannot reinterpret memory!", LOGHEAD);
                    break;
                }

                if (obj->status == core::datas::OrderStatus::INIT) {
                    core::datas::TradeOperateResult result = self.trade->order(*obj);
                    obj->status = core::datas::OrderStatus::ACCEPTED;
                    if (result.code != 0) {
                        obj->status = core::datas::OrderStatus::REJECTED;
                        memset(obj->msg, 0, core::datas::ORDER_MSG_MAX_SIZE);
                        memcpy(obj->msg, result.msg.c_str(), core::datas::ORDER_MSG_MAX_SIZE);
                        // obj->msg[core::datas::ORDER_MSG_MAX_SIZE - 1] = '\0';
                        break;
                    }
                    core::order::Order* order_obj = core::order::Order::get_instance();
                    order_obj->insert(obj->client_id, command_pair);
                    obj->client_id = order_obj->next_client_id();
                }
                break;
            }
            case core::datas::CommandType::CANCEL: {
                core::datas::CancelObj *obj = reinterpret_cast<core::datas::CancelObj*>(command_pair.first);
                if (obj) {
                    core::datas::TradeOperateResult result = self.trade->cancel(*obj);
                    obj->status = core::datas::OrderStatus::ACCEPTED;
                    if (result.code != 0) {
                        obj->status = core::datas::OrderStatus::REJECTED;
                        memset(obj->msg, 0, core::datas::ORDER_MSG_MAX_SIZE);
                        memcpy(obj->msg, result.msg.c_str(), core::datas::ORDER_MSG_MAX_SIZE);
                        // obj->msg[core::datas::ORDER_MSG_MAX_SIZE - 1] = '\0';
                        break;
                    }
                } else {
                    spdlog::error("{} CANCEL cannot reinterpret memory!", LOGHEAD);
                    break;
                }
                break;
            }
            default:
                spdlog::error("{} unknow command type: {}", LOGHEAD, static_cast<int64_t>(command_pair.first->command_type));
                break;
            }
        }
    }
}

void Modules::interval_1s() {
    spdlog::info("{}", LOGHEAD);
}

void Modules::add_instrument(core::datas::Instruments *instrument) {
    // if (!instrument) { return; }
    self.map_instruments[instrument->symbol] = instrument;
}

core::datas::Instruments* Modules::find_instrument(std::string symbol) {
    if (self.map_instruments.find(symbol) != self.map_instruments.end()) {
        return self.map_instruments[symbol];
    }
    return nullptr;
}

bool Modules::save_instruments() {
    std::string instruments_path = (core::util::config_path() / std::filesystem::path("instruments.json")).string();
    std::ofstream file(instruments_path);

    if (file.is_open()) {
        std::vector<core::datas::Instruments> vector_instruments(self.map_instruments.size());
        nlohmann::ordered_json  j = nlohmann::ordered_json::array();
        for (const auto& pair: self.map_instruments) {
            nlohmann::ordered_json instrumentJson;
            pair.second->to_json(instrumentJson);
            j.push_back(instrumentJson);
        }

        file << j.dump(4);
        file.close();
        spdlog::info("{} save instruments success!", LOGHEAD);
        return true;
    }

    spdlog::error("{} cannot open {}", LOGHEAD, instruments_path);
    return false;

}

} // namespace core::modules

#undef LOGHEAD