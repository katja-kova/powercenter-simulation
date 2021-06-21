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
#include <cmath>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "powercenter.pb.h"
#include "powercenter.grpc.pb.h"
#include "myMosq.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Server;
using grpc::ServerContext;
using grpc::ServerBuilder;
using grpc::Status;

using powercenter::ParticipantService;
using powercenter::DataRequest;
using powercenter::DataResponse;

class Participant {

public:
    Participant();
    std::string getHostIP();
    void setValue(std::string id, std::string value);
    std::string createJsonObj();
    int UDPsendData(std::string data);
    int MQTTsendData(std::string data);
private:
    int kwCurrent;
    int kwEnvMax;
    int kwEnvMin;
    int kwSetMax;
    double kwFluct;
    int sendLimited;
    int MsgNr;
    std::string mode, id, ip, port;
    mqtt_client* mqtt;
    int generateValue();
    static std::string getCurrentTimestamp();
};

#endif //VS_PARTICIPANT_H
