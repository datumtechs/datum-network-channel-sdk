#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "api_manager.h"
// #include "event_exception.h"

namespace py = pybind11;
// 定义模块名称(pyio)
PYBIND11_MODULE(pyio, m) {

  // 导出IChannel接口; 注意：IChannel对象已经导出, 在Rosetta或PSI中不能再导出，否则会有冲突;
  py::class_<IChannel, unique_ptr<IChannel, py::nodelete>>(m, "IChannel");

  py::class_<APIManager>(m, "APIManager")
    .def(py::init<>())
    .def("create_channel", &APIManager::create_channel)
    .def("Recv", &APIManager::Recv)
    .def("Send", &APIManager::Send)
    ;

  // py::register_exception<EventException>(m, "PyExp");
}
