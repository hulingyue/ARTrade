#pragma once
#include <string>
#include <cstring>

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

enum class InstrumentType {
      SPOT    // 现货
    , Future  // 期货 | 合约
    , Option  // 期权
    , Bond    // 债券
};

struct Instruments {
    InstrumentType type;
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
};

} // namespace core::datas
