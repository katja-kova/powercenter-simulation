//
// Created by katja on 01.06.21.
//

#include <iostream>
#include <string>
#include <thread>
#include "Participant.h"
#include "rpcserver.h"

#define PARAM_COUNT_MIN 4
#define PARAM_COUNT_MAX 6


void runRPCServer(Participant *participant) {
    char* ge = getenv("PARTICIPANT_RPC_PORT");
    if(ge == NULL) {
        std::cerr << "ERROR: Cannot read env-variable: PARTICIPANT_RPC_PORT" << std::endl;
        return;
    }
    std::string srv_addr("0.0.0.0:"+std::string(getenv("PARTICIPANT_RPC_PORT")));
    rpcServer* server = new rpcServer(participant);
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder b;
    b.AddListeningPort(srv_addr, grpc::InsecureServerCredentials());
    b.RegisterService(server);
    std::unique_ptr<Server> srv(b.BuildAndStart());
    std::cout << "INFO: Participant RPC Server started and listening on " << srv_addr << std::endl;
    srv->Wait();
    delete server;
}

int main(int argc, char *argv[]) {
    std::string mode, ip, port, id;

    Participant *participant = new Participant();
    //split off RPC-related stuff
    std::thread t = std::thread(runRPCServer, participant);
    t.detach();

    if (argc > PARAM_COUNT_MIN && argc <= PARAM_COUNT_MAX) {

        // read variables from docker-compose.yml
        // initialize a new participant
        mode = argv[1];
        id = argv[2];
        ip = argv[3];
        port = argv[4];

        std::cout << "creating new participant..." << std::endl;
        std::cout << "mode: " + mode << std::endl;
        std::cout << "id: " + id << std::endl;
        std::cout << "host ip: " + participant->getHostIP() << std::endl;
        std::cout << "destination ip: " + ip << std::endl;
        std::cout << "destination port: " + port << std::endl;

        try {

            if(mode == "Producer"){
                participant->createProducer(ip, port, mode, id);
            } else if (mode == "Consumer"){
                participant->createConsumer(ip, port, mode, id);
            } else {
                std::cout << "ERROR: participant's mode unrecognized" << std::endl;
            }
        } catch (char const *c) {
            std::cout << "ERROR " << std::endl;
            std::cout << c << std::endl;
        }
    } else {
        std::cout << "ERROR: attributes unavailable "<< std::endl;
    }
    return 0;
}
