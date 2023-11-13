#pragma once
#include <string>

namespace core::base::datas {
static const int SUBSCRIBE_MAX_SIZE = 10;
static const int SYMBOL_MAX_LENGTH = 16;

struct MarketOperateResult {
    int code;
    std::string msg;
};

struct TradeOperateResult {
    int code;
    std::string msg;
};


enum class MessageType : char {
      ShareMemory
    , WebSocket
    , Socket
};

enum class Identity : char {
      Master = 'M'
    , Slave = 'S'
};


enum class CommandType : char {
      SUBSCRIBE
    , UNSUBSCRIBE
    , ORDER
    , CANCEL
};

enum class MarketType: char {
      Depth
    , Kline
    , Bbo
};

enum class CommandStatus : char {
      APANDING
    , FILLED
};

enum class OrderSide : char {
      BUY
    , SELL
};

enum class OrderOffset : char {
      OPEN
    , CLOSE
    , CLOSETODAY
    , CLOSEYESTERDAY
};

enum class OrderType : char {
      MARKET
    , LIMIT
    , STOPLOSS
    , TAKEPROFIT
};

enum class OrderTIF : char {
      GTC // good till canceled
    , IOC // immediately or cancel
    , FOK // full or kill
};

enum class OrderStatus {
      INIT // strategy create order
    , PANDING  // system process order
    , ACCEPTED // exchange accept order
    , REJECTED // exchange reject order
    , PARTIALLYFILLED // partially filled
    , FILLED // filled
    , CANCEL // cancel
};

// 256 bytes
struct MarketObj {
    char symbol[SYMBOL_MAX_LENGTH];
    char exchange[SYMBOL_MAX_LENGTH];
    MarketType market_type;
    // 7 bytes
    long time;

    double price;
    double volumn;

    double asks[10];
    double bids[10];

    double high;
    double low;
    double open;
    double close;
};

// 160 bytes
struct SubscribeObj {
    char symbols[SUBSCRIBE_MAX_SIZE][SYMBOL_MAX_LENGTH];
};

// 64bytes
struct OrderObj {
    char symbol[SYMBOL_MAX_LENGTH];
    char exchange[SYMBOL_MAX_LENGTH];

    OrderSide side;
    OrderOffset offset;
    OrderType type;
    OrderStatus status;

    int client_id;
    int order_id;
    // space 4 bytes

    double price;
    double volumn;
};


// 36 bytes
struct CancelObj {
    char symbol[SYMBOL_MAX_LENGTH];
    char exchange[SYMBOL_MAX_LENGTH];

    int order_id;
};


// 168 bytes
struct CommandObj {
    CommandType type;
    // space 7 bytes
    union {
        SubscribeObj symbols;
        OrderObj order;
        CancelObj cancel;
    };
};


} // namespace core::base::datas
