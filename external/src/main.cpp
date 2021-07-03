#include <iostream>
#include <string>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include "powercenter.grpc.pb.h"

#include "rpcchistory.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using powercenter::History;
using powercenter::ReqHistoryMessage;
using powercenter::ReplyHistoryMessage;

int main(int argc, char *argv[]) {

	//generate common stuff
	char* target_IP = getenv("REQUEST_TARGET_IP");
	char* target_Port = getenv("REQUEST_TARGET_PORT");
	char* pageSize_cstr = getenv("RESPONSE_PAGE_SIZE");
	int pageSize = 10000;
	if(target_IP == NULL || target_Port == NULL) {
		std::cerr << "[ERROR]\tCannot get required env-Variables! Please check, that 'REQUEST_TARGET_IP' and 'REQUEST_TARGET_PORT' are set!\n";
		return 1;
	}
	if(pageSize_cstr == NULL) {
		std::cerr << "[WARN]\tCannot read env-variable 'RESPONSE_PAGE_SIZE', using default value of 10000.";
	} else {
		pageSize = stoi(std::string(pageSize_cstr));
	}
	std::string Target = std::string(target_IP)+":"+std::string(target_Port);
	std::cerr << "Getting data from " << Target << std::endl;
	rpcHistoryClient HistoryServer(grpc::CreateChannel(Target, grpc::InsecureChannelCredentials()));
	int numberOfPages = 0;
	//measure time!
	std::chrono::steady_clock::time_point begin, end;
	begin = std::chrono::steady_clock::now();
	std::cout << HistoryServer.getHistory(pageSize, &numberOfPages) << std::endl;
	end = std::chrono::steady_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs] needed to get the history (" << numberOfPages << " pages).\n";

	return 0;
}
