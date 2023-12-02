
namespace strategy::datas {

static const int SUBSCRIBE_MAX_SIZE = 10;
static const int SYMBOL_MAX_LENGTH = 16;
static const int MARKET_MAX_DEPTH = 10;

struct TradePair {
    double price;
    double quantity;
};

struct MarketResponseBase {
    char symbol[SYMBOL_MAX_LENGTH];
    char exchange[SYMBOL_MAX_LENGTH];
    unsigned long time;
};

struct MarketResponseBbo : public MarketResponseBase {
    double price;
    double quantity;
};

struct MarketResponseDepth : public MarketResponseBase {
    TradePair newest;

    TradePair asks[MARKET_MAX_DEPTH];
    TradePair bids[MARKET_MAX_DEPTH];
};

struct MarketResponseKline : public MarketResponseBase {
    double high;
    double low;
    double open;
    double close;
};

} // strategy::datas