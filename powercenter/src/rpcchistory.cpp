#include "rpcchistory.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using powercenter::History;
using powercenter::ReqHistoryMessage;
using powercenter::ReplyHistoryMessage;

std::string rpcHistoryClient::getHistory(int pageSize, int* numOfPages) {
	std::string history, page;
	int pageNo=0;
	do {
		page = "";
		requestPage(&page, pageSize, pageNo++);
		history += page;
	} while(page.size() > 0);
	*numOfPages = pageNo-1;
	//std::cout << pageNo << " pages with " << pageSize << " Bytes each.\n";
	return history;
}
void rpcHistoryClient::requestPage(std::string* result, int pageSize, int pageNo) {
	ClientContext context;
	ReplyHistoryMessage reply;
	ReqHistoryMessage request;
	request.set_pagesize(pageSize);
	request.set_pageno(pageNo);
	Status s = this->stub_->GetHistory(&context, request, &reply);
	if(!s.ok()) {
		std::cerr << "[ERROR]\trpc failed!\n";
		return;
	}
	/*if(!reply->has_history()) {
		std::cerr << "[ERROR]\tno history element!\n";
		return;
	}// */
	*result = reply.history();
}
