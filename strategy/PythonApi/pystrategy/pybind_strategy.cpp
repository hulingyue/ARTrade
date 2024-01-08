#include <pybind11/pybind11.h>
#include <core/datas.hpp>
#include "strategy.h"


namespace py = pybind11;

class PyStrategy : public Strategy {
public:
    using Strategy::Strategy;

    std::string project_name() override {
        PYBIND11_OVERLOAD_PURE(std::string, Strategy, project_name,);
    }

    core::datas::MessageType message_type() override {
        PYBIND11_OVERLOAD_PURE(core::datas::MessageType, Strategy, message_type);
    }

    void task() override {
        PYBIND11_OVERLOAD_PURE(void, Strategy, task,);
    }

    void on_market() override {
        PYBIND11_OVERLOAD_PURE(void, Strategy, on_market);
    }

    void on_traded() override {
        PYBIND11_OVERLOAD_PURE(void, Strategy, on_traded);
    }

    void on_order() override {
        PYBIND11_OVERLOAD_PURE(void, Strategy, on_order);
    }
};

PYBIND11_MODULE(pystrategy, m) {
    py::class_<PyStrategy, Strategy>(m, "PyStrategy")
        .def(py::init<>())
        .def("project_name", &PyStrategy::project_name)
        .def("message_type", &PyStrategy::message_type)
        .def("task", &PyStrategy::task)
        .def("on_market_bbo", &PyStrategy::on_market_bbo, py::arg("Market_bbo"))
        .def("on_market_depth", &PyStrategy::on_market_depth, py::arg("Market_depth"))
        .def("on_market_kline", &PyStrategy::on_market_kline, py::arg("Market_kline"))
        .def("on_market", &PyStrategy::on_market)
        .def("on_traded", &PyStrategy::on_traded)
        .def("on_order", &PyStrategy::on_order)
        .def("subscribe", &PyStrategy::subscribe)
        .def("unsubscribe", &PyStrategy::unsubscribe)
        .def("order", &PyStrategy::order)
        .def("cancel", &PyStrategy::cancel)
        .def("run", &PyStrategy::run);
}
