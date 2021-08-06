// file sync_client.cc
#include "sync_client.h"
#include <fstream>
#include <sstream>
#include <grpc++/security/credentials.h>
#if USE_BUFFER_
#include "simple_buffer.h"
#endif
#include <thread>
#include <chrono>   
using namespace chrono;

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

SyncClient::SyncClient(const ViaInfo& via_info, const string& taskid)
{
	task_id_ = taskid;
  std::shared_ptr<grpc::ChannelCredentials> creds;

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
	// cout << "create channel, server_addr:" << server_addr << endl;
}

ssize_t SyncClient::send(const string& self_nodeid, const string& remote_nodeid, 
    const string& msg_id, const char* data, const size_t nLen, int64_t timeout)
{
	// cout << "SyncClient::send, remote_nodeid:" << remote_nodeid << ", msg_id:" << msg_id
  //   << ", data length:" << nLen << endl;
	
  SendRequest req_info;
  // 发送客户端的nodeid到服务器
  req_info.set_nodeid(self_nodeid);

#if USE_BUFFER_
  simple_buffer buffer(msg_id, data, nLen);
  req_info.set_data((const char*)buffer.data(), buffer.len());
#else
  req_info.set_id(msg_id);
  req_info.set_data(data, nLen);
#endif
  
  RetCode ret_code;

  auto start_time = system_clock::now();
  auto end_time   = start_time;
  int64_t elapsed = 0;
  system_clock::time_point deadline = start_time + std::chrono::milliseconds(timeout);
  do {
    grpc::ClientContext context;
    // 添加注册到via的参数
    // context.AddMetadata("node_id", remote_nodeid);
    context.AddMetadata("task_id", task_id_);
    context.AddMetadata("party_id", remote_nodeid);

    // 设置阻塞等待和超时时间
    context.set_wait_for_ready(true);
    context.set_deadline(deadline);
    Status status = stub_->Send(&context, req_info, &ret_code);
    if (status.ok()) 
    {
      // cout << "send data to " << remote_nodeid << " succeed=====" << endl;
      break;
    } 
    else 
    {
      end_time = system_clock::now();
      elapsed = duration_cast<duration<int64_t, std::milli>>(end_time - start_time).count();

      // cout << "send data to " << remote_nodeid << " failed, cost " 
      //   << elapsed << " milliseconds." << endl;
      if(elapsed >= timeout)
      {
          cout << "send request timeout!" << endl;
          return 0;
      }
    }
  } while(true);
	
	return nLen;
}
