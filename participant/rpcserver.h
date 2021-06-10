//
// Created by katja on 28.05.21.
//

#ifndef RPCSERVER_H
#define RPCSERVER_H

#include <string>
#include <stdio.h>
#include <cstdio>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>
#include "powercenter.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include "Participant.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

using powercenter::ParticipantService;
using powercenter::DataRequest;
using powercenter::DataResponse;

class rpcServer final : public ::powercenter::ParticipantService::Service {
private:
    Participant* participant;
public:
    rpcServer(Participant* participant) {
        this->participant = participant;
    };
    Status GetData(ServerContext* context, const DataRequest* request, DataResponse* response) override{

        std::string id = request->id();
        std::string value = request->value();
        this->participant->setValue(id, value);
        response->set_result("success");
        return Status::OK;
    };
    ~rpcServer(){};
};

#endif //RPCSERVER_H