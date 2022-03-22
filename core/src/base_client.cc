// file base_client.cc
#include "base_client.h"
#include <IceGrid/IceGrid.h>
#include <Glacier2/Glacier2.h>

class CloseCallbackI : public Ice::CloseCallback
{
public:
    virtual void closed(const Ice::ConnectionPtr&)
    {
        cout << "The Glacier2 session has been destroyed." << endl;
    }
};

BaseClient::~BaseClient()
{ 
	if(ptr_communicator_) 
		ptr_communicator_->shutdown();
}

BaseClient::BaseClient(const ViaInfo& via_info, const string& taskid)
{
	remote_nid_ = via_info.id;
	// set properties
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties();
	task_id_ = taskid;
	string protocol = "tcp";
	bool is_ssl = false;
	if(!via_info.ca_cert_path_.empty()) {
		protocol = "ssl";
		is_ssl = true;
	}

	// set ssl property
	auto SetSSLProperty = [&initData](const ViaInfo& via_info) -> bool {
		if(via_info.server_cert_path_.empty() || via_info.client_cert_path_.empty()) {
			return false;
		}
		initData.properties->setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL");
        initData.properties->setProperty("IceSSL.DefaultDir", via_info.cert_dir_);
        initData.properties->setProperty("IceSSL.CAs", via_info.ca_cert_path_);
        initData.properties->setProperty("IceSSL.CertFile", via_info.client_cert_path_);
        initData.properties->setProperty("IceSSL.Password", via_info.password_);

		return true;
	};

	string serAddress = via_info.via_address;
	int npos = serAddress.find(":");
	// 服务器地址为空，走Glacier2路由
	if(-1 == npos) {
		const string& ip = via_info.glacier2_info.Ip_;
		const string& port = via_info.glacier2_info.Port_;
		const string& app_name = via_info.glacier2_info.AppName_;
		if(app_name.empty() || ip.empty() || port.empty()) {
			HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_NO_GLACIER2, taskid, via_info.id.c_str());
		}

		// 设置Glacier2的路由信息, app_name为Glacier2服务名称, ip和port分别为Glacier2服务监听的地址以及端口
		string strGlacier2Cfg = app_name + "/router:" + protocol + " -p " + port + " -h " + ip;
		// cout << "strGlacier2Cfg:" << strGlacier2Cfg << endl;
		initData.properties->setProperty(C_Glacier2_Router_Key, strGlacier2Cfg);
		initData.properties->setProperty(C_MAX_SIZE_KEY, "0");
		if(is_ssl && !SetSSLProperty(via_info)) {
			HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_INVALID_CERT, taskid, via_info.id.c_str());
		}

		try 
		{
			ptr_holder_ = make_shared<Ice::CommunicatorHolder>(initData);
			ptr_communicator_ = ptr_holder_->communicator();
			
			string servantAdapterId = C_Servant_Adapter_Id_Prefix + taskid + "_" + via_info.id;
			string servantId = C_Servant_Id_Prefix + "_" + via_info.id;
			// 寻找方式
			string strProxy = servantId + "@" + servantAdapterId;
			Glacier2::RouterPrx router = Glacier2::RouterPrx::checkedCast(ptr_communicator_->getDefaultRouter());
			Glacier2::SessionPrx session = router->createSession("", "");

			Ice::Int acmTimeout = router->getACMTimeout();
			Ice::ConnectionPtr connection = router->ice_getCachedConnection();
			assert(connection);
			connection->setACM(acmTimeout, IceUtil::None, Ice::HeartbeatAlways);
			connection->setCloseCallback(new CloseCallbackI());

			stub_ = IoChannelPrx::uncheckedCast(ptr_communicator_->stringToProxy(strProxy));
		}
      	catch (const Ice::Exception& e) 
		{
			HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_CREATE_CLIENT, taskid, via_info.id.c_str(), e.what());
		}
	} else {
		// 直连
		string ip = serAddress.substr(0, npos);
		string port = serAddress.substr(npos+1, serAddress.length());
		string servantId = C_Servant_Id_Prefix + "_" + via_info.id;
		string endpoints = servantId + ":" + protocol + " -h " + ip + " -p " + port;
		initData.properties->setProperty(C_Server_Proxy_Key, endpoints);
		initData.properties->setProperty(C_MAX_SIZE_KEY, "0");
		if(is_ssl && !SetSSLProperty(via_info)) {
			HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_INVALID_CERT, taskid, via_info.id.c_str());
		}

		try 
		{
			ptr_holder_ = make_shared<Ice::CommunicatorHolder>(initData);
			ptr_communicator_ = ptr_holder_->communicator();	
			// uncheckedCast 函数从不进行远程调用
			stub_ = Ice::uncheckedCast<IoChannelPrx>(
				ptr_communicator_->propertyToProxy(C_Server_Proxy_Key)->ice_twoway()->
					ice_timeout(-1)->ice_secure(false));
		} 
		catch (const Ice::Exception& e) 
		{
			HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_CREATE_CLIENT, taskid, via_info.id.c_str(), e.what());
		}
	}
	if (!stub_)
		HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_INVALID_PROXY, taskid, via_info.id.c_str());
}
bool BaseClient::CheckConnStatus(const uint64_t conn_timeout, const useconds_t usec) 
{
	timer_.start();
	do
	{
		try 
		{
			cout << "Attempt to connect to the remote node:" << remote_nid_ << endl;
		#if STATIC_CALL
			stub_->ice_ping();
		#else
			Ice::ByteSeq inParams, outParams;
			stub_->ice_invoke("ping", Ice::Normal, inParams, outParams);
		#endif
		}
		catch (const Ice::Exception& ex) 
		{
			// cerr << ex << endl;
			if(timer_.ms_elapse() >= conn_timeout)
			{        
				HANDLE_EXCEPTION_EVENT(C_EVENT_CODE_CONNECT_TIMEOUT, task_id_, remote_nid_.c_str(), conn_timeout);
				// return 0;
			}
			usleep(usec);
			continue;
		}
		break;
    } while(true);

	return true;
}
