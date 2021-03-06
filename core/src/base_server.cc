// file base_server.cc
#include "base_server.h"
#include "const.h"

BaseServer::BaseServer(const NodeInfo& server_info, const string& taskId)
{
	taskId_ = taskId;
	// set properties
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	string serAddress = server_info.address;
	int npos = serAddress.find(":");
	string protocol = "tcp";
	bool is_ssl = false;
	if(!server_info.ca_cert_path_.empty()) {
		protocol = "ssl";
		is_ssl = true;
		cout << "The Channel SDK uses SSL to communicate." << endl;
	}
	if(-1 == npos) {
		const string& ip = server_info.ice_grid_info.Ip_;
		const string& port = server_info.ice_grid_info.Port_;
		const string& app_name = server_info.ice_grid_info.AppName_;
		if(app_name.empty() || ip.empty() || port.empty()) {
			HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_NO_ICEGRID, taskId, server_info.id.c_str());
		}
		// 设置IceGrid的服务信息（用于servant信息的注册; IceGrid不需要使用ssl）
        // string value_endpoints = "ChannelIceGrid/Locator:tcp -p 10032 -h 192.168.2.128";
		string value_endpoints = app_name + "/Locator:tcp -p " + port + " -h " + ip;
		// cout << "value_endpoints:" << value_endpoints << endl;
        initData.properties->setProperty(C_IceGrid_Locator_Key, value_endpoints);
        string servantAdapterId = C_Servant_Adapter_Id_Prefix + taskId + "_" + server_info.id;
		FilterIllChar(servantAdapterId);
		if(server_info.public_ip_.empty()) {
			HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_NO_PUBLIC_IP, taskId, server_info.id.c_str());
		}

		string servant_endpoints = protocol + " -h " + server_info.public_ip_;
		initData.properties->setProperty(C_Servant_Endpoints_Key, servant_endpoints);
        initData.properties->setProperty(C_Servant_AdapterId_Key, servantAdapterId);
	} else {
		// 直连
		string ip = serAddress.substr(0, npos);
		string port = serAddress.substr(npos+1, serAddress.length());
		string endpoints = protocol + " -h " + ip + " -p " + port;
		initData.properties->setProperty(C_Servant_Endpoints_Key, endpoints);
	}

	// There is no limit to the size of the received message
	initData.properties->setProperty(C_MAX_SIZE_KEY, "0");
	// initData.properties->setProperty("Ice.Trace.Protocol", "1");
	// initData.properties->setProperty("Ice.Trace.ThreadPool", "1");

	// set ssl properties
	if(is_ssl) {
		initData.properties->setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL");
        initData.properties->setProperty("IceSSL.DefaultDir", server_info.cert_dir_);
        initData.properties->setProperty("IceSSL.CAs", server_info.ca_cert_path_);
        initData.properties->setProperty("IceSSL.CertFile", server_info.server_cert_path_);
        initData.properties->setProperty("IceSSL.Password", server_info.password_);
	}

	try 
	{
		ptr_holder_ = make_shared<Ice::CommunicatorHolder>(initData);
		ptr_communicator_ = ptr_holder_->communicator();
		ptr_adapter_ = ptr_communicator_->createObjectAdapter(C_Servant_Adapter_Name);
	} 
	catch (const Ice::Exception& e) 
	{
		HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_START_SERVICE, taskId, server_info.id.c_str(), e.what());
	}
}
