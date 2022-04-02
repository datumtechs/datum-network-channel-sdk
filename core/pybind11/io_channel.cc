#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;
#include "api_manager.h"

// 定义模块名称(grpc)
PYBIND11_MODULE(grpc, m) {

  py::class_<IChannel,  unique_ptr<IChannel, py::nodelete> >(m, "IChannel");

  py::class_<APIManager>(m, "APIManager")
    .def(py::init<>())
    .def("create_channel", &APIManager::create_channel)
    .def("recv_msg", &APIManager::recv_msg)
    .def("send_msg", &APIManager::send_msg)
    ;
}
