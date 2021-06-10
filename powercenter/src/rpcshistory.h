#ifndef RPC_HISTORY_SERVER_H
#define RPC_HISTORY_SERVER_H

#include <string>
#include <stdio.h>
#include <cstdio>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>
#include "powercenter.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include "util.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

using powercenter::History;
using powercenter::ReqHistoryMessage;
using powercenter::ReplyHistoryMessage;

class rpcHistoryServer final : public ::powercenter::History::Service {
public:
	rpcHistoryServer(){};
	Status GetHistory(ServerContext* context, const ReqHistoryMessage* request, ReplyHistoryMessage* reply) override{
	        long page = request->pageno();
	        const long pageSize = request->pagesize() + 1;
	        long offset = page * (pageSize-1);
	        char buf[pageSize] = {0};
	        FILE* f = fopen("data/history", "r");
	        fseek(f, offset, SEEK_SET);
	        fgets(buf, pageSize, f);
	        reply->set_history(std::string(buf));
	        return Status::OK;
	}
	~rpcHistoryServer(){};
};
#endif // RPC_HISTORY_SERVER_H
