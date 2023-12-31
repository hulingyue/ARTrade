#include <pybind11/pybind11.h>
#include "../include/busy_loop.h"


namespace py = pybind11;

// PYBIND11_MODULE(pystrategy, m) {
//     py::class_<Strategy>(m, "Strategy")
//         .def(py::init<>())
//         .def("project_name", &Strategy::project_name)
//         .def("message_type", &Strategy::message_type)
//         .def("task", &Strategy::task)
//         // .def("on_market_bbo", &Strategy::on_market_bbo)
//         // .def("on_market_depth", &Strategy::on_market_depth)
//         // .def("on_market_kline", &Strategy::on_market_kline)
//         .def("on_market", &Strategy::on_market)
//         .def("on_traded", &Strategy::on_traded)
//         .def("on_order", &Strategy::on_order)
//         // .def("subscribe", &Strategy::subscribe)
//         // .def("unsubscribe", &Strategy::unsubscribe)
//         .def("order", &Strategy::order)
//         .def("cancel", &Strategy::cancel)
//         .def("run", &Strategy::run);
// }
