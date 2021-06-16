#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;
// 类文件
#include "../include/io_channel_impl.h"

// 定义模块名称(io_channel)
PYBIND11_MODULE(io_channel, m) {
  m.doc() = R"pbdoc(
      Rosetta backend entry for python API.
    )pbdoc";

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

  py::class_<IChannel,  shared_ptr<IChannel> >(m_impl, "IChannel")
  .def("recv", &IChannel::Recv)
  .def("send", &IChannel::Send)
  .def("get_data_node_ids", &IChannel::GetDataNodeIDs)
  .def("get_computation_node_ids", &IChannel::GetComputationNodeIDs)
  .def("get_result_node_ids", &IChannel::GetResultNodeIDs)
  .def("get_current_node_id", &IChannel::GetCurrentNodeID)
  .def("get_connected_node_ids", &IChannel::GetConnectedNodeIDs)
  ;

  // 用于测试：send/recv
  py::class_<GRpcChannel, shared_ptr<GRpcChannel>, IChannel>(m_impl, "GRpcChannel")
    .def(py::init<shared_ptr<BasicIO>>())
    .def("send", &GRpcChannel::Send)
    .def("recv", &GRpcChannel::Recv)
    .def("GetCurrentVia", &GRpcChannel::GetCurrentVia)
    .def("GetCurrentAddress", &GRpcChannel::GetCurrentAddress)
    .def("GetTaskId", &GRpcChannel::GetTaskId)
    .def("GetDataNodeIDs", &GRpcChannel::GetDataNodeIDs)
    .def("GetComputationNodeIDs", &GRpcChannel::GetComputationNodeIDs)
    .def("GetResultNodeIDs", &GRpcChannel::GetResultNodeIDs)
    .def("GetCurrentNodeID", &GRpcChannel::GetCurrentNodeID)
    .def("GetConnectedNodeIDs", &GRpcChannel::GetConnectedNodeIDs)
	  ;
}
