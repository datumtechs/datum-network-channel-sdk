// file async_server.cc
#include "async_server.h"
#if USE_BUFFER_
#include "simple_buffer.h"
#endif
#include <thread>
#include <chrono>   
using namespace chrono;

void CallData::Proceed(void* ptr_save, void* ptr_mtx, void* ptr_cv)
{
	if (status_ == CREATE)
	{
		status_ = PROCESS;
		service_->RequestSend(&ctx_, &request_, &responder_, cq_, cq_, this);
	}
	else if (status_ == PROCESS)
	{
		// cout << "thread id： " << std::this_thread::get_id() << endl;
		new CallData(service_, cq_);
		status_ = FINISH;
		const string& nodeId = request_.nodeid();
	#if MULTI_LOCKS
		map<string, shared_ptr<mutex>>* ptr_map_mtx = static_cast<map<string, shared_ptr<mutex>>*>(ptr_mtx);
		auto iter_mtx = ptr_map_mtx->find(nodeId);
		if(iter_mtx == ptr_map_mtx->end())
		{
			reply_.set_code(RET_SUCCEED_CODE);
			responder_.Finish(reply_, Status::OK, this);
			return;
		}
		std::unique_lock<mutex> guard(*(*ptr_map_mtx)[nodeId]);
	#else
		std::unique_lock<mutex> guard(*static_cast<mutex*>(ptr_mtx));
	#endif
		// 返回值
		reply_.set_code(RET_SUCCEED_CODE);
		responder_.Finish(reply_, Status::OK, this);
	#if USE_CACHE
		#if MULTI_LOCKS
			map<string, shared_ptr<queue<SendRequest>>>* ptr_map_send_queue = 
				static_cast<map<string, shared_ptr<queue<SendRequest>>>*>(ptr_save);

			map<string, shared_ptr<condition_variable>>* ptr_map_cv = 
				static_cast<map<string, shared_ptr<condition_variable>>*>(ptr_cv);
			// 缓存请求数据
			(*ptr_map_send_queue)[nodeId]->push(request_);
			// 通知处理线程
			(*ptr_map_cv)[nodeId]->notify_all();
		#else
			queue<SendRequest>* ptr_send_queue = static_cast<queue<SendRequest>*>(ptr_save);
			condition_variable* ptr_single_cv = static_cast<condition_variable*>(ptr_cv);
			// 缓存请求数据
			ptr_send_queue->push(request_);
			// 通知处理线程
			ptr_single_cv->notify_all();
		#endif
	#else
		// 保存数据
		map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map = 
			static_cast<map<string, shared_ptr<ClientConnection>>*>(ptr_save);
		// cout << "nodeid:===" << nodeId << endl;
		auto iter = ptr_client_conn_map->find(nodeId);
		if(iter == ptr_client_conn_map->end())
		{
			return;
		}
		// The msgid is already included in the data  
		const string& data = request_.data();

		#if USE_BUFFER_
			iter->second->buffer_->write(data.data(), data.size());
		#else
			const string& msgid = request_.id();
			iter->second->write(msgid, data);
		#endif
	#endif
	}
}

bool AsyncServer::close()
{
    // 关闭服务
    server_->Shutdown();
	// Always shutdown the completion queue after the server.
	for(int i = 0; i < thread_count_; ++i)
	{
		map_cq_[i]->Shutdown();
	}
	return true;
}

AsyncServer::~AsyncServer()
{
	close();
}

AsyncServer::AsyncServer(const string& server_address, 
	map<string, shared_ptr<ClientConnection>>* ptr_client_conn_map)
{
	ptr_client_conn_map_ = ptr_client_conn_map;

#if MULTI_LOCKS
	for(auto &v : *ptr_client_conn_map)
	{
		cout << "nodeid key:" << v.first << endl;
		map_mtx_[v.first] = make_shared<mutex>();
		#if USE_CACHE
			map_cv_[v.first] = make_shared<condition_variable>();
			map_send_queue_[v.first] = make_shared<queue<SendRequest>>();
		#endif	
	}
#endif

	// ServerBuilder builder;
	builder_ = make_shared<ServerBuilder>();
	builder_->SetMaxReceiveMessageSize(INT_MAX);
	// Listen on the given address without any authentication mechanism.
	builder_->AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service_" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *asynchronous* service.
	builder_->RegisterService(&service_);
	// Get hold of the completion queue used for the asynchronous communication
	// with the gRPC runtime.
	int enableCPUNum = sysconf(_SC_NPROCESSORS_ONLN);
	thread_count_ = enableCPUNum > 1 ? (enableCPUNum - 1): 1;
#ifdef THREAD_COUNT
	if(0 >= THREAD_COUNT)
	{
		thread_count_ = 1;
	}
	else if(thread_count_ > THREAD_COUNT)
	{
		thread_count_ = THREAD_COUNT;
	}
	else
	{
		// nothing to do
	}
#endif
	for(int i = 0; i < thread_count_; ++i)
	{
		map_cq_[i] = builder_->AddCompletionQueue();
	}

	// Finally assemble the server.
	server_ = builder_->BuildAndStart();
	cout << "Server listening on " << server_address << endl;

	// Proceed to the server's main loop.
	// Spawn a new CallData instance to serve new clients.

	for(int i = 0; i < thread_count_; ++i)
	{
		new CallData(&service_, map_cq_[i].get());
	}
}

void AsyncServer::Handle_Event(const int numEvent)
{
	auto iter = map_cq_.find(numEvent);
	if(iter == map_cq_.end())
	{
		return;
	}
	
	void* tag;  // uniquely identifies a request.
    bool ok;
	std::unique_ptr<ServerCompletionQueue> cq = move(map_cq_[numEvent]);
	while (true) 
	{
		GPR_ASSERT(cq->Next(&tag, &ok));
		GPR_ASSERT(ok);
		#if MULTI_LOCKS
			#if USE_CACHE
				static_cast<CallData*>(tag)->Proceed(&map_send_queue_, &map_mtx_, &map_cv_);
			#else
				static_cast<CallData*>(tag)->Proceed(ptr_client_conn_map_, &map_mtx_);
			#endif
		#else 
			#if USE_CACHE
				static_cast<CallData*>(tag)->Proceed(&send_queue_, &mtx_, &cv_);
			#else
				static_cast<CallData*>(tag)->Proceed(ptr_client_conn_map_, &mtx_);
			#endif
		#endif
	}
}


#if USE_CACHE
#if MULTI_LOCKS
void AsyncServer::Handle_Data(const string& nodeid)
{
	do {
		std::unique_lock<std::mutex> lck(*map_mtx_[nodeid]);
		while(!map_send_queue_[nodeid]->empty())
		{
			const SendRequest& req_info = map_send_queue_[nodeid]->front();
			
			auto iter = ptr_client_conn_map_->find(nodeid);
			/*
			if(iter == ptr_client_conn_map_->end())
			{
				map_send_queue_[nodeid]->pop();
				continue;
			}
			*/

			iter->second->write(req_info.id(), req_info.data());
			map_send_queue_[nodeid]->pop();
		}
		// cout << "cv wait, nodeid:" << nodeid << endl;
		(*map_cv_[nodeid]).wait(lck);
	} while(true);
}
#else 
void AsyncServer::Handle_Data()
{
	do {
		std::unique_lock<std::mutex> lck(mtx_);
		while(!send_queue_.empty())
		{
			const SendRequest& req_info = send_queue_.front();
			
			auto iter = ptr_client_conn_map_->find(req_info.nodeid());
			/*
			if(iter == ptr_client_conn_map_->end())
			{
				map_send_queue_[nodeid]->pop();
				continue;
			}
			*/

			iter->second->write(req_info.id(), req_info.data());
			send_queue_.pop();
		}
		// cout << "cv wait============" << endl;
		cv_.wait(lck);
	} while(true);
}
#endif
#endif
