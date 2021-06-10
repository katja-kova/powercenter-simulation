//
// Created by katja on 29.04.21.
//

#ifndef VS_PARTICIPANT_H
#define VS_PARTICIPANT_H

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sys/types.h>
#include <cstring>
#include <locale>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <unistd.h>
#include <chrono>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <random>
#include <sys/types.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "powercenter.pb.h"
#include "powercenter.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Server;
using grpc::ServerContext;
using grpc::ServerBuilder;
using grpc::Status;

using powercenter::ParticipantService;
using powercenter::DataRequest;
using powercenter::DataResponse;

static int nr=0;
static int conscurr=0; static int consmax=1;
static int windcurr=0; static int windmax=1;
static int solcurr=0; static int solmax=1;
static int nuclcurr=0; static int nuclmax=1;

class Participant {

public:
    std::string getHostIP();
    static void setValue(std::string id, std::string value);
    int generateValue(int max, int min);
    void createConsumer (std::string ip, std::string port, std::string mode, std::string id);
    void createProducer (std::string ip, std::string port, std::string mode, std::string id);

private:
    static std::string getCurrentTimestamp();
    std::string createJsonObj(int nr, const std::string id, const std::string mode, int kw);
    int UDPsendData(const std::string &ip, const std::string &port, std::string data);

};

#endif //VS_PARTICIPANT_H
