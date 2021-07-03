#ifndef RPC_CLIENT_HISTORY_H
#define RPC_CLIENT_HISTORY_H

#include <string>
#include <grpcpp/grpcpp.h>
#include "powercenter.grpc.pb.h"
#include "powercenter.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using powercenter::History;
using powercenter::ReqHistoryMessage;
using powercenter::ReplyHistoryMessage;

class rpcHistoryClient {
public:
	rpcHistoryClient(std::shared_ptr<Channel> channel) : stub_(History::NewStub(channel)) {}
	std::string getHistory(int pageSize, int* numberOfPages);
	const std::unique_ptr<History::Stub> &getStub() const {
		return stub_;
	}
private:
	std::unique_ptr<History::Stub> stub_;
	void requestPage(std::string* result, int pageSize, int pageNo);
};
#endif //RPC_CLIENT_HISTORY_H
