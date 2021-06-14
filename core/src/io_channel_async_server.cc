/*
 *
 * Copyright 2021 io_channel authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "protos/io_channel.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using io_channel::IoChannel;
using io_channel::SendRequest;
using io_channel::RetCode;
using namespace std;

class IoChannelAsyncServer final 
{
public:
  bool close()
  {
    server_->Shutdown();
    cq_->Shutdown();
  }

  map<string, string>& get_data_map()
  {
      return save_data_map_;
  }

  bool wait()
  {
      // Proceed to the server's main loop.
      HandleRpcs();
      return true;
  }

  ~IoChannelAsyncServer() 
  {
    server_->Shutdown();
    // Always shutdown the completion queue after the server.
    cq_->Shutdown();
  }

  IoChannelAsyncServer(const string& server_addr) 
  {
    ServerBuilder builder;
    builder.SetMaxReceiveMessageSize(INT_MAX);
    builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_addr << std::endl;

    // Proceed to the server's main loop.
    // HandleRpcs();
  }

 private:
  class CallData 
  {
  public:
    CallData(IoChannel::AsyncService* service, ServerCompletionQueue* cq, 
          map<string, string>* save_data_map)
        : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE), p_save_data_map_(save_data_map) 
    {
      // Invoke the serving logic right away.
      Proceed();
    }

    void Proceed() 
    {
      if (status_ == CREATE) 
      {
        // Make this instance progress to the PROCESS state.
        status_ = PROCESS;
        // As part of the initial CREATE state, we *request* that the system
        // start processing SayHello requests. In this request, "this" acts are
        // the tag uniquely identifying the request (so that different CallData
        // instances can serve different requests concurrently), in this case
        // the memory address of this CallData instance.
        service_->RequestSend(&ctx_, &request_, &responder_, cq_, cq_, this);
      } 
      else if (status_ == PROCESS) 
      {
        // Spawn a new CallData instance to serve new clients while we process
        // the one for this CallData. The instance will deallocate itself as
        // part of its FINISH state.
        new CallData(service_, cq_, p_save_data_map_);
        // The actual processing.
        cout << "request_.nodeid:" << request_.nodeid() << ", request_.id:" << request_.id()
          << ", request.data: " << request_.data().length() << endl;

        string strSaveId = request_.nodeid() + ":" + request_.id();

        std::lock_guard<std::mutex> guard(mtx_);
        if(p_save_data_map_)
          p_save_data_map_->insert(std::pair<string, string>(strSaveId, request_.data()));
          cout << "save_data_map_.size == " << p_save_data_map_->size() << endl;

        reply_.set_code(100);
        // And we are done! Let the gRPC runtime know we've finished, using the
        // memory address of this instance as the uniquely identifying tag for
        // the event.
        status_ = FINISH;
        responder_.Finish(reply_, Status::OK, this);
      } 
      else 
      {
        GPR_ASSERT(status_ == FINISH);
        // Once in the FINISH state, deallocate ourselves (CallData).
        delete this;
      }
    }

   private:
    mutex mtx_;
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    IoChannel::AsyncService* service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue* cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;

    // What we get from the client.
    SendRequest request_;
    // What we send back to the client.
    RetCode reply_;

    // The means to get back to the client.
    ServerAsyncResponseWriter<RetCode> responder_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;  // The current serving state.

    map<string, string>* p_save_data_map_;
  };

  // This can be run in multiple threads if needed.
  void HandleRpcs() 
  {
    // Spawn a new CallData instance to serve new clients.
    new CallData(&service_, cq_.get(), &save_data_map_);
    void* tag;  // uniquely identifies a request.
    bool ok;
    while (true) 
    {
      // Block waiting to read the next event from the completion queue. The
      // event is uniquely identified by its tag, which in this case is the
      // memory address of a CallData instance.
      // The return value of Next should always be checked. This return value
      // tells us whether there is any kind of event or cq_ is shutting down.
      GPR_ASSERT(cq_->Next(&tag, &ok));
      // GPR_ASSERT(ok);
      static_cast<CallData*>(tag)->Proceed();
    }
  }

  unique_ptr<ServerCompletionQueue> cq_;
  IoChannel::AsyncService service_;
  unique_ptr<Server> server_;
  map<string, string> save_data_map_;
};

/*
int main(int argc, char** argv) {
  MyAsyncServerImpl server;
  server.Run();

  return 0;
}
*/