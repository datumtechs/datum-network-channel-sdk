#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;
#include "api_manager.h"

// 定义模块名称(ice)
PYBIND11_MODULE(ice, m) {

#if USE_ALONE
  // 单独使用时需要导出IChannel接口; 因为在Rosetta中IChannel对象已经导出, 所以再导出会有冲突;
  py::class_<IChannel,  shared_ptr<IChannel> >(m, "IChannel");
#endif

  py::class_<APIManager>(m, "APIManager")
    .def(py::init<>())
    .def("create_channel", &APIManager::create_channel)
    .def("recv_msg", &APIManager::recv_msg)
    .def("send_msg", &APIManager::send_msg)
    ;
}
