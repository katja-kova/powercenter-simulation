//
// Created by katja on 24.05.21.
//

#include <string>

#include <grpcpp/grpcpp.h>
#include "powercenter.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using powercenter::ParticipantService;
using powercenter::DataRequest;
using powercenter::DataResponse;

class RPCclient {
public:
    RPCclient(std::shared_ptr<Channel> channel) : stub_(powercenter::ParticipantService::NewStub(channel)){};

    void getData(std::string id, std::string value) {

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // data we're sending to the Participant
        DataRequest request;
        // data we expect from the Participant
        DataResponse response;

        request.set_id(id);
        request.set_value(value);

        Status status = this->stub_->GetData(&context, request, &response);

        if (status.ok()) {
            std::cout << "INFO: grpc call successful:"<< response.result() << std::endl;

        } else {
            std::cout<<"gRPC failed with:" << status.error_code() << ": " << status.error_message() << std::endl;

        }
    }
    const std::unique_ptr<ParticipantService::Stub> &getStub() const {
        return stub_;
    }
private:
    std::unique_ptr<powercenter::ParticipantService::Stub> stub_;

};
