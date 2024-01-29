#pragma once
#include <string>
#include <cstring>
#include <nlohmann/json.hpp>

namespace core::datas {
static const int SUBSCRIBE_MAX_SIZE = 10;
static const int SYMBOL_MAX_LENGTH = 16;
static const uint64_t SYMBOL_MAX_CAPACITY = 100;
static const int MARKET_MAX_DEPTH = 10;
static const int ORDER_MSG_MAX_SIZE = 256;
static const int ORDERID_MAX_SIZE = 128;

struct MarketOperateResult {
    int code;
    std::string msg;
};

struct TradeOperateResult {
    int code;
    std::string msg;
};


struct TradePair {
    double price;
    double quantity;
};

enum class MessageType : int64_t {
      ShareMemory = 1
    , WebSocket = 2
    , ZeroMQ = 3
};

enum class Identity : int64_t {
      Master = 1
    , Slave = 2
};

enum class CommandStatus : int64_t {
      EFFECTIVE // 生效中
    , INVALID // 失效
};

enum class OrderSide : int64_t {
      UNKNOW = -1
    , BUY = 1
    , SELL = 2
};

enum class OrderOffset : int64_t {
      UNKNOW = -1
    , OPEN = 1
    , CLOSE = 2
    , CLOSETODAY = 3
    , CLOSEYESTERDAY = 4
};

enum class OrderType : int64_t {
      UNKNOW = -1
    , MARKET = 1
    , LIMIT = 2
    , STOPLOSS = 3
    , TAKEPROFIT = 4
};

enum class OrderTIF : int64_t {
      UNKNOW = -1
    , GTC = 1 // good till canceled
    , IOC = 2 // immediately or cancel
    , FOK = 3 // full or kill
};

enum class OrderStatus : int64_t {
      INIT = 1 // strategy create order
    , PANDING = 2  // system process order
    , ACCEPTED = 3 // exchange accept order
    , REJECTED = 4 // exchange reject order
    , PARTIALLYFILLED = 5 // partially filled
    , FILLED = 6 // filled
    , CANCEL = 7 // cancel
};

/******************************************/
/***    Market - begin                  ***/
/******************************************/
enum class MarketType: int64_t {
      Unknow = -1
    , Depth = 11
    , Kline = 12
    , Bbo = 13
};

struct Market_base {
    MarketType market_type;
    char symbol[SYMBOL_MAX_LENGTH] = {0};
    char exchange[SYMBOL_MAX_LENGTH] = {0};
    uint64_t time;
};

struct alignas(64) Market_bbo : public Market_base {
    double price;
    double quantity;
};

struct alignas(64) Market_depth : public Market_base {
    double price;
    double quantity;

    TradePair asks[MARKET_MAX_DEPTH];
    TradePair bids[MARKET_MAX_DEPTH];
};

struct alignas(64) Market_kline : public Market_base {
    bool confirm;
    char interval[8];
    uint64_t start;
    uint64_t end;

    double high;
    double low;
    double open;
    double close;
};

/******************************************/
/***    Market - end                    ***/
/******************************************/


/******************************************/
/***    Command - begin                 ***/
/******************************************/
enum class CommandType : int64_t {
      UNKNOW = -1
    , SUBSCRIBE = 21
    , UNSUBSCRIBE = 22
    , ORDER = 23
    , CANCEL = 24
};

struct Command_base {
    CommandType command_type;
};

// subscribe & unsubscribe
struct alignas(64) SymbolObj : public Command_base {
    char symbols[SYMBOL_MAX_CAPACITY][SYMBOL_MAX_LENGTH] = {0};

    bool push_back(const char* symbol) {
        if (_size >= SYMBOL_MAX_CAPACITY) { return false; }
        std::strncpy(symbols[size()], symbol, SYMBOL_MAX_LENGTH - 1);
        _size++;
        return true;
    }

    uint64_t size() {
        return _size;
    }

    uint64_t capacity() {
        return SYMBOL_MAX_CAPACITY;
    }

private:
    uint64_t _size = 0;    
};

struct alignas(64) OrderObj : public Command_base {
    char symbol[SYMBOL_MAX_LENGTH] = {0};
    char exchange[SYMBOL_MAX_LENGTH] = {0};
    char msg[ORDER_MSG_MAX_SIZE] = {0};

    OrderSide side;
    OrderOffset offset;
    OrderType type;
    OrderStatus status;
    OrderTIF tif;

    uint64_t client_id;
    char order_id[ORDERID_MAX_SIZE];

    double price;
    double quantity; // order origin count
    double traded; // turnover 
    double revoked; // success cancel turnover
};

struct alignas(64) CancelObj : public Command_base {
    char symbol[SYMBOL_MAX_LENGTH] = {0};
    char exchange[SYMBOL_MAX_LENGTH] = {0};
    char msg[ORDER_MSG_MAX_SIZE] = {0};

    int client_id;
    int order_id;

    OrderStatus status;
};

/******************************************/
/***    Command - end                   ***/
/******************************************/


/******************************************/
/****   ShareMemory Data Header - begin ***/
/******************************************/
struct alignas(64) MarketDataHeader {
    MarketType type;
    size_t data_size = 0;
};

struct alignas(64) CommandDataHeader {
    CommandType type;
    size_t data_size = 0;
    CommandStatus status = CommandStatus::INVALID;
};
/******************************************/
/****   ShareMemory Data Header - end   ***/
/******************************************/

struct InstrumentType {
    enum class Type {
          UNKNOW    // 未知
        , SPOT      // 现货
        , Future    // 期货 | 合约
        , Option    // 期权
        , Bond      // 债券
    };

    static std::string to_string(Type type) {
        switch (type) {
        case Type::SPOT: { return "SPOT"; }
        case Type::Future: { return "Future"; }
        case Type::Option: { return "Option"; }
        case Type::Bond: { return "Bond"; }
        default: { return "UNKNOW"; }
        }
    }

    static Type from_string(std::string type) {
        if ("SPOT" == type) { return Type::SPOT; }
        else if ("Future" == type) { return Type::Future; }
        else if ("Option" == type) { return Type::Option; }
        else if ("Bond" == type) { return Type::Bond; }
        else { return Type::UNKNOW; }
    }
};

struct Instruments {
    InstrumentType::Type type;
    std::string symbol;
    int log_size;
    double price_scale;

    double min_leverage;
    double max_leverage;
    double leverage_scale;

    double min_quantity;
    double max_quantity;
    double quantity_scale;

    std::string expire;

    void to_json(nlohmann::ordered_json &j) const {
        j["type"] = InstrumentType::to_string(type);
        j["symbol"] = symbol;
        j["log_size"] = log_size;
        j["price_scale"] = price_scale;
        j["min_leverage"] = min_leverage;
        j["max_leverage"] = max_leverage;
        j["leverage_scale"] = leverage_scale;
        j["min_quantity"] = min_quantity;
        j["max_quantity"] = max_quantity;
        j["quantity_scale"] = quantity_scale;
        j["expire"] = expire;
    }

    void from_json(const nlohmann::json& j) {
        InstrumentType::from_string(j.at("type").get<std::string>());
        j.at("symbol").get_to(symbol);
        j.at("log_size").get_to(log_size);
        j.at("price_scale").get_to(price_scale);
        j.at("min_leverage").get_to(min_leverage);
        j.at("max_leverage").get_to(max_leverage);
        j.at("leverage_scale").get_to(leverage_scale);
        j.at("min_quantity").get_to(min_quantity);
        j.at("max_quantity").get_to(max_quantity);
        j.at("quantity_scale").get_to(quantity_scale);
        j.at("expire").get_to(expire);
    }
};

} // namespace core::datas
