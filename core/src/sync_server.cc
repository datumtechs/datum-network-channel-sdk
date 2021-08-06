// file sync_server.cc
#include "sync_server.h"
#if USE_BUFFER_
#include "simple_buffer.h"
#endif
#include <fstream>
#include <sstream>

// 服务器
bool SyncServer::close()
{
    // 关闭服务
    if(server_)
        server_->Shutdown();
}

bool SyncServer::wait()
{
    if(server_)
        server_->Wait();
    cout << "SyncServer::wait()" << endl;
}

static string get_file_contents(const string& fpath)
{
    ifstream ifile(fpath);
    if(!ifile.good())
    {
        cout << "file is not exist:" << fpath << endl;
        return "";
    }
    ostringstream buf;
    char ch;
    while(buf&&ifile.get(ch))
    buf.put(ch);
    return buf.str();
}

SyncServer::SyncServer(const NodeInfo& server_info, map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map)
{
    ptr_client_conn_map_ = ptr_client_conn_map;
    grpc::ServerBuilder builder;
    // 调整消息大小限制(默认最大为4M, 设置最大为2G)
    builder.SetMaxReceiveMessageSize(INT_MAX);
    shared_ptr<grpc::ServerCredentials> creds;

    #if(1 == SSL_TYPE)  
	{   // openssl
		if(server_info.ca_cert_path_.empty() || server_info.server_key_path_.empty() || server_info.server_cert_path_.empty())
		{
			cerr << "Invalid server openssl certificate, please check!" << endl;
			return;
		}
		auto str_root_crt = get_file_contents(server_info.ca_cert_path_.c_str()); // for verifying clients
    	auto str_server_key = get_file_contents(server_info.server_key_path_.c_str());
    	auto str_server_cert = get_file_contents(server_info.server_cert_path_.c_str());
		grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp = {str_server_key.c_str(), str_server_cert.c_str()};
		grpc::SslServerCredentialsOptions ssl_opts(GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY);
		ssl_opts.pem_root_certs = str_root_crt.c_str();
		ssl_opts.pem_key_cert_pairs.push_back(pkcp);
		creds = grpc::SslServerCredentials(ssl_opts);
	}
	#elif(2 == SSL_TYPE)  
    {	// gmssl
        if(server_info.ca_cert_path_.empty() || server_info.server_sign_key_path_.empty() || 
		   server_info.server_sign_cert_path_.empty() || server_info.server_enc_key_path_.empty() ||
		   server_info.server_enc_cert_path_.empty() )
        { 
            cerr << "Invalid server gmssl certificate, please check!" << endl;
            return;
        }

        grpc::SslServerCredentialsOptions::PemKeyCertPair sig_pkcp = {
			server_info.server_sign_key_path_.c_str(), server_info.server_sign_cert_path_.c_str()};

        grpc::SslServerCredentialsOptions::PemKeyCertPair enc_pkcp = {
			server_info.server_enc_key_path_.c_str(), server_info.server_enc_cert_path_.c_str()};
        grpc::SslServerCredentialsOptions ssl_opts(GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY);
        ssl_opts.pem_root_certs = server_info.ca_cert_path_.c_str();
        ssl_opts.pem_key_cert_pairs.push_back(sig_pkcp);
        ssl_opts.pem_key_cert_pairs.push_back(enc_pkcp);
        creds = grpc::SslServerCredentials(ssl_opts);
    }
	#else
	{
		creds = grpc::InsecureServerCredentials();
	}
    #endif

    builder.AddListeningPort(server_info.address, creds);
    builder.RegisterService(this);
    server_ = builder.BuildAndStart();
    cout << "Server listening on " << server_info.address << endl;
    // server_->Wait();
}

grpc::Status SyncServer::Send(grpc::ServerContext* context, const SendRequest* request, 
    RetCode* response)
{
    // cout << "SyncServer::Send, send request nodeid:" << nodeId << ", msgid:" << msgid << endl;
    // std::unique_lock<mutex> guard(mtx_);
    const string& nodeId = request->nodeid();
    // const string& msgid = request->id();

    auto iter = ptr_client_conn_map_->find(nodeId);
    if(iter == ptr_client_conn_map_->end())
    {
        return Status::OK;
    }
    // The msgid is already included in the data  
    const string& data = request->data();
#if USE_BUFFER_
    iter->second->buffer_->write(data.data(), data.size());
#else
    const string& msgid = request->id();
    iter->second->write(msgid, data);
#endif
    // simple_buffer buffer(msgid, data.data(), data.size());
    // client_conn->buffer_->write((const char*)buffer.data(), buffer.len());

    response->set_code(RET_SUCCEED_CODE);
    return Status::OK;
}
