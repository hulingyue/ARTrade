#include "busy_loop.h"
#include <core/message.hpp>


namespace {
struct Self {
    core::message::Message* message = nullptr;
};

using namespace core::base::datas;


static inline bool market(core::message::Message* message, std::vector<std::string> symbols, bool is_subscribe) {
    std::vector<std::string> container;
    bool status = true;

    auto market_event = [&](std::vector<std::string> symbols, bool is_subscribe) {
        CommandObj obj;
        obj.type = is_subscribe ? CommandType::SUBSCRIBE : CommandType::UNSUBSCRIBE;
        for (std::size_t index = 0; index < symbols.size(); index++) {
            std::strncpy(obj.symbols.symbols[index], symbols[index].c_str(), SYMBOL_MAX_LENGTH);
            obj.symbols.symbols[index][15] = '\0';
        }

        status &= message->write_command(obj);
    };

    for (std::size_t count = 0; count < symbols.size(); count++) {

        if (container.size() >=SUBSCRIBE_MAX_SIZE) {
            market_event(container, is_subscribe);
        }

        container.emplace_back(std::move(symbols[count]));
    }

    if (container.size() > 0) {
        market_event(container, is_subscribe);
    }

    return status;
}
} // namespace 


Strategy::Strategy() : self( *new Self{} ){
}

Strategy::~Strategy() {
    delete &self;
}

bool Strategy::subscribe(std::vector<std::string> symbols) {
    if (!self.message) { return false; }
    return market(self.message, std::move(symbols), true);
}

bool Strategy::unsubscribe(std::vector<std::string> symbols) {
    if (!self.message) { return false; }
    return market(self.message, std::move(symbols), false);
}

bool Strategy::order() {
    return false;
}

bool Strategy::cancel() {
    return false;
}

void Strategy::run() {
    custom_init();

    std::thread task_thread(std::bind(&Strategy::task, this));
    task_thread.detach();

    while (true) {
        // read commands

        // read market

        // 
    }
}

void Strategy::custom_init() {
    std::string proj = project_name();
   MessageType type = message_type();
    self.message = new core::message::Message(proj, type,Identity::Slave);

}