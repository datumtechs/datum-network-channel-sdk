// file base_client.cc
#include "base_client.h"
#include <IceGrid/IceGrid.h>
#include <Glacier2/Glacier2.h>
#include <thread>
#include <chrono>   
using namespace chrono;

class CloseCallbackI : public Ice::CloseCallback
{
public:
    virtual void closed(const Ice::ConnectionPtr&)
    {
        cout << "The Glacier2 session has been destroyed." << endl;
    }
};

// bool BaseClient::MakeCredentials(const ViaInfo& via_info)
// {
// 	#if(1 == SSL_TYPE)
// 	{
// 		if(via_info.server_cert_path_.empty() || via_info.client_key_path_.empty() || 
// 		   via_info.client_cert_path_.empty())
// 		{
// 			gpr_log(GPR_ERROR, "Invalid client openssl certificate, please check!");
// 			return false;
// 		}
// 		auto str_root_crt = get_file_contents(via_info.server_cert_path_); // for verifying clients
// 		auto str_client_key = get_file_contents(via_info.client_key_path_);
// 		auto str_client_cert = get_file_contents(via_info.client_cert_path_);

// 		grpc::SslCredentialsOptions ssl_opts;
// 		ssl_opts.pem_root_certs  = str_root_crt.c_str();
// 		ssl_opts.pem_private_key = str_client_key.c_str();
// 		ssl_opts.pem_cert_chain  = str_client_cert.c_str();
// 		creds_ = grpc::SslCredentials(ssl_opts);
// 	}
// 	#elif(2 == SSL_TYPE) 
// 	{
// 		if(via_info.server_cert_path_.empty() || via_info.client_sign_key_path_.empty() || 
// 		via_info.client_sign_cert_path_.empty() || via_info.client_enc_key_path_.empty() ||
// 		via_info.client_enc_cert_path_.empty())
// 		{
// 			gpr_log(GPR_ERROR, "Invalid client gmssl certificate, please check!");
// 			return false;
// 		}

// 		grpc::SslCredentialsOptions ssl_opts;
// 		ssl_opts.pem_root_certs  = via_info.server_cert_path_.c_str();
// 		ssl_opts.pem_private_key = via_info.client_sign_key_path_.c_str();
// 		ssl_opts.pem_cert_chain  = via_info.client_sign_cert_path_.c_str();
// 		ssl_opts.pem_enc_private_key =  via_info.client_enc_key_path_.c_str();
// 		ssl_opts.pem_enc_cert_chain = via_info.client_enc_cert_path_.c_str();

// 		creds_ = grpc::SslCredentials(ssl_opts);
// 	}	
// 	#else
// 	{
// 		creds_ = grpc::InsecureChannelCredentials();
// 	}
// 	#endif

// 	return true;
// }

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
	string serAddress = via_info.via_address;
	int npos = serAddress.find(":");
	// 服务器地址为空，走Glacier2路由
	if(-1 == npos) {
		const string& ip = via_info.glacier2_info.Ip_;
		const string& port = via_info.glacier2_info.Port_;
		const string& app_name = via_info.glacier2_info.AppName_;
		if(app_name.empty() || ip.empty() || port.empty()) {
			string strErrMsg = "The service node: " + via_info.id + " doesn't configure Glacier2 address!";
			cout << strErrMsg << endl;
			throw (strErrMsg);
		}

		// 设置Glacier2的路由信息, app_name为Glacier2服务名称, ip和port分别为Glacier2服务监听的地址以及端口
		string strGlacier2Cfg = app_name + "/router:tcp -p " + port + " -h " + ip;
		// cout << "strGlacier2Cfg:" << strGlacier2Cfg << endl;
		initData.properties->setProperty(C_Glacier2_Router_Key, strGlacier2Cfg);
		initData.properties->setProperty("Ice.MessageSizeMax", "0");
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
	} else {
		// 直连
		string ip = serAddress.substr(0, npos);
		string port = serAddress.substr(npos+1, serAddress.length());
		string servantId = C_Servant_Id_Prefix + "_" + via_info.id;
		string endpoints = servantId + ":tcp -h " + ip + " -p " + port;
		initData.properties->setProperty(C_Server_Proxy_Key, endpoints);
		initData.properties->setProperty("Ice.MessageSizeMax", "0");
		ptr_holder_ = make_shared<Ice::CommunicatorHolder>(initData);
		ptr_communicator_ = ptr_holder_->communicator();
		
		// uncheckedCast 函数从不进行远程调用
		stub_ = Ice::uncheckedCast<IoChannelPrx>(
			ptr_communicator_->propertyToProxy(C_Server_Proxy_Key)->ice_twoway()->
				ice_timeout(-1)->ice_secure(false));
	}
	if (!stub_)
		throw "Invalid proxy";
}

bool BaseClient::CheckByStaticCall(const uint64_t conn_timeout, const useconds_t usec)
{
	auto start_time = system_clock::now();
	auto end_time   = start_time;
	int64_t elapsed = 0;
	do
	{
		try 
		{
			cout << "Attempt to connect to the remote node:" << remote_nid_ << endl;
			stub_->ice_ping();
		}
		catch (const Ice::Exception& ex) 
		{
			// cerr << ex << endl;
			end_time = system_clock::now();
			elapsed = duration_cast<duration<int64_t, std::milli>>(end_time - start_time).count();
			if(elapsed >= conn_timeout)
			{        
				string strErrMsg = "connect to remote nodeid:" + remote_nid_ + " timeout, The timeout period is: " + 
				to_string(conn_timeout) + "ms.";
				cout << strErrMsg << endl;
				throw (strErrMsg);
				// return 0;
			}
			usleep(usec);
			continue;
		}
		break;
    } while(true);

	return true;
}

bool BaseClient::CheckByDynamicCall(const uint64_t conn_timeout, const useconds_t usec)
{
	auto start_time = system_clock::now();
	auto end_time   = start_time;
	int64_t elapsed = 0;
	do
	{
		int status = 0;
		try 
		{
			cout << "Attempt to connect to the remote node:" << remote_nid_ << endl;
			Ice::ByteSeq inParams, outParams;
			if(!stub_->ice_invoke("ping", Ice::Normal, inParams, outParams))
			{
				cout << "Ping to remote node:" << remote_nid_ << " failed, wait..." << endl;
				status = 1;
			}
		}
		catch (const Ice::Exception& ex) 
		{
			cerr << ex << endl;
			status = 1;
		}

		if(0 == status)
		{
			break;
		}
		else 
		{
			end_time = system_clock::now();
			elapsed = duration_cast<duration<int64_t, std::milli>>(end_time - start_time).count();
			if(elapsed >= conn_timeout)
			{        
				string strErrMsg = "connect to remote nodeid:" + remote_nid_ + " timeout, The timeout period is: " + 
				to_string(conn_timeout) + "ms.";
				cout << strErrMsg << endl;
				throw (strErrMsg);
			}
			usleep(usec);
			continue;
		}
		
    } while(true);

	return true;
}

// BaseClient::BaseClient(const NodeInfo& node_info, const string& taskid)
// {
// 	task_id_ = taskid;
// 	ViaInfo via_info;
// 	via_info.via_address = node_info.via_address;
	
// 	#if(1 == SSL_TYPE)
// 		via_info.server_cert_path_ = node_info.ca_cert_path_;
// 		via_info.client_key_path_ = node_info.client_key_path_;
// 		via_info.client_cert_path_ = node_info.client_cert_path_;
// 	#elif(2 == SSL_TYPE) 
// 		via_info.server_cert_path_ = node_info.ca_cert_path_;
// 		via_info.client_sign_key_path_ = node_info.client_sign_key_path_;
// 		via_info.client_sign_cert_path_ = node_info.client_sign_cert_path_;
// 		via_info.client_enc_key_path_ = node_info.client_enc_key_path_;
// 		via_info.client_enc_cert_path_ = node_info.client_enc_cert_path_;
// 	#endif

// 	if(!MakeCredentials(via_info)){return;}
// 	auto channel = grpc::CreateChannel(via_info.via_address, creds_);
// 	via_stub_ = VIAService::NewStub(channel);
// }

// bool BaseClient::SignUpToVia(const NodeInfo& server_info)
// {
// 	if(nullptr == via_stub_)
// 	{
// 		gpr_log(GPR_ERROR, "uninitialized stub to connect to the VIA service, please initialize!");
// 		return false;
// 	}

// 	grpc::ClientContext context;
// 	context.AddMetadata("task_id", task_id_);
// 	context.AddMetadata("party_id", server_info.id);

// 	SignupReq reg_req;
// 	Boolean ret_code;
// 	reg_req.set_taskid(task_id_);
// 	reg_req.set_partyid(server_info.id);
// 	reg_req.set_servicetype("");
// 	reg_req.set_address(server_info.address);

// 	via_stub_->Signup(&context, reg_req, &ret_code);
// 	if (false == ret_code.result()) 
// 	{
// 		gpr_log(GPR_ERROR, "Signup via server failed, via server address:%s!", server_info.via_address.c_str());
// 		return false;
// 	}    
// 	return true;
// }
