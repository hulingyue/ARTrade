#pragma once
#include <iostream>
#include "pimpl.h"
#include "sharememory.hpp"


#define LOGHEAD "[Message::" + std::string(__func__) + "]"


namespace {

using namespace core::datas;
using namespace core::sharememory;


/***********************************/
/** CLASS: BaseMessage            **/
/***********************************/
class BaseMessage {
public:
    BaseMessage(std::string proj_name, Identity identity)
     : proj_name(proj_name)
     , identity(identity) {}
    virtual ~BaseMessage() {}

public:
    virtual bool write_market(const MarketObj obj) = 0;
    virtual MarketObj* read_market(uint32_t id) = 0;
    virtual bool write_command(const CommandObj obj) = 0;
    virtual CommandObj* read_command(uint32_t id) = 0;
    virtual bool write_response(const ResponseObj obj) = 0;
    virtual ResponseObj* read_response(uint32_t id) = 0;

public:
    std::string proj_name;
    Identity identity;
};


/***********************************/
/** CLASS: ShareMemoryMessage     **/
/***********************************/
class ShareMemoryMessage : public BaseMessage {
public:
    ShareMemoryMessage(std::string proj_name, Identity identity) : 
        BaseMessage(proj_name, identity)
        , share_memory_market(new ShareMemory<MarketObj>(proj_name + "_market.txt", 1, memory_type::market))
        , share_memory_command(new ShareMemory<CommandObj>(proj_name + "_command.txt", 2, memory_type::command))
        , share_memory_response(new ShareMemory<ResponseObj>(proj_name + "_response.txt", 3, memory_type::response)) {
            if (identity == Identity::Master) {
                share_memory_market->create();
                share_memory_command->create();
                share_memory_response->create();
            } else if (identity == Identity::Slave) {
                share_memory_market->connect();
                share_memory_command->connect();
                share_memory_response->connect();
            } else {
                spdlog::error("{} unknow identity: {}", LOGHEAD, static_cast<char>(identity));
                exit(-1);
            }
        }
    
    virtual ~ShareMemoryMessage() {
        if (share_memory_market) { delete share_memory_market; }
        if (share_memory_command) { delete share_memory_command; }
    }

public:
    virtual bool write_market(const MarketObj obj) override {
        return share_memory_market->write(obj, false, true);
    }

    virtual MarketObj* read_market(uint32_t id) override {
        return share_memory_market->read_next(id);
    }

    virtual bool write_command(const CommandObj obj) override {
        return share_memory_command->write(obj, true, false);
    }

    virtual CommandObj* read_command(uint32_t id) override {
        return share_memory_command->read_next(id);
    }

    virtual bool write_response(const ResponseObj obj) override {
        return share_memory_response->write(obj, true, false);
    }

    virtual ResponseObj* read_response(uint32_t id) override {
        return share_memory_response->read_next(id);
    }

private:
    // ShareMemory
    ShareMemory<MarketObj>* share_memory_market;
    ShareMemory<CommandObj>* share_memory_command;
    ShareMemory<ResponseObj>* share_memory_response;
};


// /***********************************/
// /** CLASS: WebSocketMessage       **/
// /***********************************/
// class WebSocketMessage : public BaseMessage {

// };


// /***********************************/
// /** CLASS: SocketMessage          **/
// /***********************************/
// class SocketMessage : public BaseMessage {

// };


/*****************************************************************
 ** STRUCT: MessageSelf                                                **
******************************************************************/
struct MessageSelf {
    std::string proj_name;
    MessageType type;
    BaseMessage* message;

    u_int32_t market_id = 0;
    u_int32_t command_id = 0;
    u_int32_t response_id = 0;
};

} // namespace 


namespace core::message {

class Message final {
public:
    Message(std::string proj_name, MessageType type, Identity identity = Identity::Slave)
     : self { *new MessageSelf{} } {
        self.proj_name = proj_name;
        self.type = type;

        if (type == MessageType::ShareMemory) {
            self.message = new ShareMemoryMessage(proj_name, identity);
        } else if (type == MessageType::WebSocket) {
            self.message = nullptr;
        } else if (type == MessageType::Socket) {
            self.message = nullptr;
        } else {
            self.message = nullptr;
        }
    }

    ~Message() {
        if (self.message) { delete self.message; }
        delete &self;
    }

public:
    // market
    bool write_market(const MarketObj obj) const {
        if (self.message) {
            return self.message->write_market(obj);
        }
        return false;
    }

    MarketObj* read_market() {
        if (self.message) {
            auto it = self.message->read_market(self.market_id);
            if (it) {
                self.market_id = (self.market_id + 1) % buffer_size;
                return it; 
            }
        }
        return nullptr;
    }

    // command
    bool write_command(const CommandObj obj) {
        if (self.message) {
            return self.message->write_command(obj);
        }
        return true;
    }

    CommandObj* read_command() {
        if (self.message) {
            auto it = self.message->read_command(self.command_id);
            if (it) {
                self.command_id = (self.command_id + 1) % buffer_size;
                return it; 
            }
        }
        return nullptr;
    }

    // response
    bool write_response(const ResponseObj obj) {
        if (self.message) {
            return self.message->write_response(obj);
        }
        return true;
    }

    ResponseObj* read_response() {
        if (self.message) {
            auto it = self.message->read_response(self.response_id);
            if (it) {
                self.response_id = (self.response_id + 1) % buffer_size;
                return it; 
            }
        }
        return nullptr;
    }


private:
    MessageSelf &self;
};

} // namespace core::message


#undef LOGHEAD
