#pragma once
#include <string>

namespace core::datas {
static const int SUBSCRIBE_MAX_SIZE = 10;
static const int SYMBOL_MAX_LENGTH = 16;
static const int MARKET_MAX_DEPTH = 10;

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
      APANDING
    , FILLED
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
    , Depth = 1
    , Kline = 2
    , Bbo = 3
};

struct Market_base {
    MarketType market_type;
    char symbol[SYMBOL_MAX_LENGTH];
    char exchange[SYMBOL_MAX_LENGTH];
    unsigned long time;
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
    , SUBSCRIBE = 1
    , UNSUBSCRIBE = 2
    , ORDER = 3
    , CANCEL = 4
};

// subscribe & unsubscribe
struct alignas(64) SymbolObj {
    char symbols[SYMBOL_MAX_LENGTH];
};

struct alignas(64) OrderObj {
    char symbol[SYMBOL_MAX_LENGTH];
    char exchange[SYMBOL_MAX_LENGTH];

    OrderSide side;
    OrderOffset offset;
    OrderType type;
    OrderStatus status;
    OrderTIF tif;

    int client_id;
    int order_id;

    double price;
    double quantity;
};

struct alignas(64) CancelObj {
    char symbol[SYMBOL_MAX_LENGTH];
    char exchange[SYMBOL_MAX_LENGTH];

    int client_id;
    int order_id;
};

/******************************************/
/***    Command - end                   ***/
/******************************************/

} // namespace core::datas
