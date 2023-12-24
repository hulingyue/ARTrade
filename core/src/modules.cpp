#include <iostream>
#include <ctime>
#include <filesystem>

#include "core/modules.h"
#include "core/config.h"
#include "core/util.h"


#define LOGHEAD "[Modules::" + std::string(__func__) + "]"

namespace {

struct Self {
    core::api::market::Market *market = nullptr;
    core::api::trade::Trade *trade = nullptr;
    core::message::message::MarketChannel *market_channel = nullptr;
    core::message::message::CommandChannel *command_channel = nullptr;

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
    }
    self.command_channel = new core::message::message::CommandChannel(proj, 40 * MB);
    if (self.trade) {
        // self.trade->set_message(self.command_channel);
        self.trade->init();
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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        auto now = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - ts) >= std::chrono::seconds(1)) {
            interval_1s();
            ts = now;
        }

        // read commands & deal with commands
        auto command = self.command_channel->read_next(address_displacement);
        if (command) {
            switch (command->command_type) {
            case core::datas::CommandType::SUBSCRIBE:{
                // very important!!!
                address_displacement = address_displacement + sizeof(core::datas::SymbolObj);

                core::datas::SymbolObj *obj = reinterpret_cast<core::datas::SymbolObj*>(command);
                uint64_t size = obj->size();

                std::vector<std::string> symbols;
                for (uint64_t index = 0; index < size; index++) {
                    symbols.emplace_back(std::move(std::string(obj->symbols[index])));
                }

                self.market->subscribe(std::vector<std::string>(std::move(symbols)));
                break;
            }
            case core::datas::CommandType::UNSUBSCRIBE: {
                // very important!!!
                address_displacement = address_displacement + sizeof(core::datas::SymbolObj);

                core::datas::SymbolObj *obj = reinterpret_cast<core::datas::SymbolObj*>(command);
                uint64_t size = obj->size();

                std::vector<std::string> symbols;
                for (uint64_t index = 0; index < size; index++) {
                    symbols.emplace_back(std::move(std::string(obj->symbols[index])));
                }

                self.market->unsubscribe(std::vector<std::string>(std::move(symbols)));
                break;
            }
            case core::datas::CommandType::ORDER: {
                // very important!!!
                address_displacement = address_displacement + sizeof(core::datas::OrderObj);
                break;
            }
            case core::datas::CommandType::CANCEL: {
                // very important!!!
                address_displacement = address_displacement + sizeof(core::datas::CancelObj);
                break;
            }
            default:
                // spdlog::error("{} unknow command type: {}", LOGHEAD, static_cast<int64_t>(command->command_type));
                break;
            }
        }
    }
}

void Modules::interval_1s() {
    spdlog::info("{}", LOGHEAD);
}

} // namespace core::modules

#undef LOGHEAD