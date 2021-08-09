// file base_client.cc
#include "base_client.h"

BaseClient::BaseClient(const ViaInfo& via_info, const string& taskid)
{
	task_id_ = taskid;
  	shared_ptr<grpc::ChannelCredentials> creds;
	#if(1 == SSL_TYPE)
	{
		if(via_info.server_cert_path_.empty() || via_info.client_key_path_.empty() || via_info.client_cert_path_.empty())
		{
		cerr << "Invalid client openssl certificate, please check!" << endl;
		return;
		}
		auto str_root_crt = get_file_contents(via_info.server_cert_path_); // for verifying clients
		auto str_client_key = get_file_contents(via_info.client_key_path_);
		auto str_client_cert = get_file_contents(via_info.client_cert_path_);

		grpc::SslCredentialsOptions ssl_opts;
		ssl_opts.pem_root_certs  = str_root_crt.c_str();
		ssl_opts.pem_private_key = str_client_key.c_str();
		ssl_opts.pem_cert_chain  = str_client_cert.c_str();
		creds = grpc::SslCredentials(ssl_opts);
	}
	#elif(2 == SSL_TYPE) 
	{
		if(via_info.server_cert_path_.empty() || via_info.client_sign_key_path_.empty() || 
		via_info.client_sign_cert_path_.empty() || via_info.client_enc_key_path_.empty() ||
		via_info.client_enc_cert_path_.empty())
		{
		cerr << "Invalid client gmssl certificate, please check!" << endl;
		return;
		}

		grpc::SslCredentialsOptions ssl_opts;
		ssl_opts.pem_root_certs  = via_info.server_cert_path_.c_str();
		ssl_opts.pem_private_key = via_info.client_sign_key_path_.c_str();
		ssl_opts.pem_cert_chain  = via_info.client_sign_cert_path_.c_str();
		ssl_opts.pem_enc_private_key =  via_info.client_enc_key_path_.c_str();
		ssl_opts.pem_enc_cert_chain = via_info.client_enc_cert_path_.c_str();
		creds = grpc::SslCredentials(ssl_opts);
	}	
	#else
	{
		creds = grpc::InsecureChannelCredentials();
	}
	#endif
	
	auto channel = grpc::CreateChannel(via_info.address, creds);
	stub_ = IoChannel::NewStub(channel);
}
