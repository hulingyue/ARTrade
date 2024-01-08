#include <string>
#include <core/datas.hpp>


/* side */
static inline std::string side_to_bybit(core::datas::OrderSide const side) {
    if (side == core::datas::OrderSide::BUY) {
        return "Buy";
    } else if (side == core::datas::OrderSide::SELL) {
        return "Sell";
    }

    return "";
}

static inline core::datas::OrderSide side_from_bybit(std::string_view const side) {
    if (side == "Buy") {
        return core::datas::OrderSide::BUY;
    } else if (side == "Sell") {
        return core::datas::OrderSide::SELL;
    }

    return core::datas::OrderSide::UNKNOW;
}


/* order type */
static inline std::string type_to_bybit(core::datas::OrderType const type) {
    if (type == core::datas::OrderType::LIMIT) {
        return "Market";
    } else if (type == core::datas::OrderType::MARKET) {
        return "Limit";
    }

    return "";
}

static inline core::datas::OrderType type_from_bybit(std::string_view const type) {
    if (type == "Market") {
        return core::datas::OrderType::MARKET;
    } else if (type == "Limit") {
        return core::datas::OrderType::LIMIT;
    }

    return core::datas::OrderType::UNKNOW;
}

/* tif */
static inline std::string tif_to_bybit(core::datas::OrderTIF const tif) {
    if (tif == core::datas::OrderTIF::GTC) {
        return "GTC";
    } else if (tif == core::datas::OrderTIF::IOC) {
        return "IOC";
    }

    return "";
}

static inline core::datas::OrderTIF tif_from_bybit(std::string_view const tif) {
    if (tif == "GTC") {
        return core::datas::OrderTIF::GTC;
    } else if (tif == "IOC") {
        return core::datas::OrderTIF::IOC;
    }

    return core::datas::OrderTIF::UNKNOW;
}