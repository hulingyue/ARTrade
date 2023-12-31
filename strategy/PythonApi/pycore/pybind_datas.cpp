#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <core/datas.hpp>

namespace py = pybind11;
using namespace core::datas;

py::str convert_char_array_to_python_string(const char* char_array) {
    return py::str(char_array);
}

template <typename T>
py::array_t<T> convert_to_numpy_array(const T *data, size_t size) {
    py::array_t<T> result({size});
    auto result_buffer = result.request();
    T *result_ptr = static_cast<T *>(result_buffer.ptr);
    std::memcpy(result_ptr, data, size * sizeof(T));
    return result;
}

template <typename T, size_t N>
std::array<T, N> convert_from_numpy_array(py::array_t<T> array) {
    auto buffer = array.request();
    if (buffer.size != N) {
        throw std::runtime_error("Array size does not match");
    }
    std::array<T, N> result;
    std::memcpy(result.data(), buffer.ptr, N * sizeof(T));
    return result;
}

PYBIND11_MODULE(pycore, m) {
    py::class_<MarketOperateResult>(m, "MarketOperateResult")
        .def(py::init<>())
        .def_readwrite("code", &MarketOperateResult::code)
        .def_readwrite("msg", &MarketOperateResult::msg);

    py::class_<TradeOperateResult>(m, "TradeOperateResult")
        .def(py::init<>())
        .def_readwrite("code", &TradeOperateResult::code)
        .def_readwrite("msg", &TradeOperateResult::msg);

    py::class_<TradePair>(m, "TradePair")
        .def(py::init<>())
        .def_readwrite("price", &TradePair::price)
        .def_readwrite("quantity", &TradePair::quantity);

    py::enum_<MessageType>(m, "MessageType")
        .value("ShareMemory", MessageType::ShareMemory)
        .value("WebSocket", MessageType::WebSocket)
        .value("ZeroMQ", MessageType::ZeroMQ)
        .export_values();

    py::enum_<Identity>(m, "Identity")
        .value("Master", Identity::Master)
        .value("Slave", Identity::Slave)
        .export_values();

    py::enum_<CommandStatus>(m, "CommandStatus")
        .value("EFFECTIVE", CommandStatus::EFFECTIVE)
        .value("INVALID", CommandStatus::INVALID)
        .export_values();

    py::enum_<OrderSide>(m, "OrderSide")
        .value("UNKNOW", OrderSide::UNKNOW)
        .value("BUY", OrderSide::BUY)
        .value("SELL", OrderSide::SELL)
        .export_values();

    py::enum_<OrderOffset>(m, "OrderOffset")
        .value("UNKNOW", OrderOffset::UNKNOW)
        .value("OPEN", OrderOffset::OPEN)
        .value("CLOSE", OrderOffset::CLOSE)
        .value("CLOSETODAY", OrderOffset::CLOSETODAY)
        .value("CLOSEYESTERDAY", OrderOffset::CLOSEYESTERDAY)
        .export_values();

    py::enum_<OrderType>(m, "OrderType")
        .value("UNKNOW", OrderType::UNKNOW)
        .value("MARKET", OrderType::MARKET)
        .value("LIMIT", OrderType::LIMIT)
        .value("STOPLOSS", OrderType::STOPLOSS)
        .value("TAKEPROFIT", OrderType::TAKEPROFIT)
        .export_values();

    py::enum_<OrderTIF>(m, "OrderTIF")
        .value("UNKNOW", OrderTIF::UNKNOW)
        .value("GTC", OrderTIF::GTC)
        .value("IOC", OrderTIF::IOC)
        .value("FOK", OrderTIF::FOK)
        .export_values();

    py::enum_<OrderStatus>(m, "OrderStatus")
        .value("INIT", OrderStatus::INIT)
        .value("PANDING", OrderStatus::PANDING)
        .value("ACCEPTED", OrderStatus::ACCEPTED)
        .value("REJECTED", OrderStatus::REJECTED)
        .value("PARTIALLYFILLED", OrderStatus::PARTIALLYFILLED)
        .value("FILLED", OrderStatus::FILLED)
        .value("CANCEL", OrderStatus::CANCEL)
        .export_values();

    /******************************************/
    /***    Market - begin                  ***/
    /******************************************/
    py::enum_<MarketType>(m, "MarketType")
        .value("Unknow", MarketType::Unknow)
        .value("Depth", MarketType::Depth)
        .value("Kline", MarketType::Kline)
        .value("Bbo", MarketType::Bbo)
        .export_values();

    py::class_<Market_base>(m, "Market_base")
        .def(py::init<>())
        .def_readwrite("market_type", &Market_base::market_type)
        .def_readwrite("time", &Market_base::time)
        .def_property("symbol",
            [](const Market_base &mb) {
                return convert_char_array_to_python_string(mb.symbol);
            },
            [](Market_base &mb, const py::str &s) {
                // Add logic to handle the conversion from Python string to C++ char array here
                strncpy(mb.symbol, s.cast<std::string>().c_str(), sizeof(mb.symbol));
            }
        )
        .def_property("exchange",
            [](const Market_base &mb) {
                return convert_char_array_to_python_string(mb.exchange);
            },
            [](Market_base &mb, const py::str &s) {
                // Add logic to handle the conversion from Python string to C++ char array here
                strncpy(mb.exchange, s.cast<std::string>().c_str(), sizeof(mb.exchange));
            }
        );
    
    py::class_<Market_bbo, Market_base>(m, "Market_bbo")
        .def(py::init<>())
        .def_readwrite("price", &Market_bbo::price)
        .def_readwrite("quantity", &Market_bbo::quantity);
    
    py::class_<Market_depth, Market_base>(m, "Market_depth")
        .def(py::init<>())
        .def_readwrite("price", &Market_depth::price)
        .def_readwrite("quantity", &Market_depth::quantity)
        .def_property("asks",
                      [](const Market_depth &md) {
                          return convert_to_numpy_array<TradePair>(md.asks, core::datas::MARKET_MAX_DEPTH);
                      },
                      [](Market_depth &md, const py::array_t<TradePair> &value) {
                          return convert_from_numpy_array<TradePair, core::datas::MARKET_MAX_DEPTH>(value);
                      })
        .def_property("bids",
                      [](const Market_depth &md) {
                          return convert_to_numpy_array<TradePair>(md.asks, core::datas::MARKET_MAX_DEPTH);
                      },
                      [](Market_depth &md, const py::array_t<TradePair> &value) {
                          return convert_from_numpy_array<TradePair, core::datas::MARKET_MAX_DEPTH>(value);
                      });

    py::class_<Market_kline, Market_base>(m, "Market_kline")
        .def(py::init<>())
        .def_readwrite("high", &Market_kline::high)
        .def_readwrite("low", &Market_kline::low)
        .def_readwrite("open", &Market_kline::open)
        .def_readwrite("close", &Market_kline::close);

}
