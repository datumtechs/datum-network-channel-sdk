#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
// 类文件
#include "include/io_channel_impl.h"

// 定义模块名称(io_channel)
PYBIND11_MODULE(io_channel, m) {
  m.doc() = R"pbdoc(
      Rosetta backend entry for python API.
    )pbdoc";

  py::class_<IChannel, shared_ptr<IChannel>>(m, "IChannel");

  // 定义绑定类
  py::module m_impl = m.def_submodule("impl");
  py::class_<IoChannelImpl>(m_impl, "api")
    .def(py::init<>())
    .def("create_channel", &IoChannelImpl::CreateChannel)
    .def("close_server", &IoChannelImpl::CloseServer)
    .def("start_server", &IoChannelImpl::StartServer)
    .def("wait_server", &IoChannelImpl::WaitServer)
	  ;

  py::class_<BasicIO, shared_ptr<BasicIO>>(m, "basicio");

  // send/recv
  py::class_<GRpcChannel, shared_ptr<GRpcChannel>>(m_impl, "grpc")
    .def(py::init<shared_ptr<BasicIO>>())
    .def("send", &GRpcChannel::Send)
    .def("recv", &GRpcChannel::Recv)
	  ;
}
