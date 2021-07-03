//
// Created by katja on 01.06.21.
//

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "Participant.h"
#include "rpcserver.h"

void runRPCServer(Participant *participant) {
    std::cout << "starting grpc server\n";
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
    Participant *participant = new Participant();
    //split off RPC-related stuff
    std::thread t = std::thread(runRPCServer, participant);
    t.detach();
    srand(time(NULL));
    std::cout << "INFO: starting data creation\n";
    try {
	char* sleepEnv = getenv("PARTICIPANT_SLEEP_DURATION");
        if(sleepEnv == NULL) {
	    std::cerr << "ERROR: missing PARTICIPANT_SLEEP_DURATION environment variable!\n";
            return -1;
        }
	int sleepDur = atoi(sleepEnv);
	if(sleepDur <1) {
            std::cerr << "ERROR: invalid value for PARTICIPANT_SLEEP_DURATION environment variable!\n";
            return -1;
	}
	std::cout << "INFO: sending data every " << sleepEnv << "s\n";
	bool useUDP = (getenv("PARTICIPANT_USE_UDP")!=NULL);
	bool useMQTT = (getenv("PARTICIPANT_USE_MQTT")!=NULL);
	if(!(useUDP || useMQTT)) {
	    std::cerr << "ERROR: Using neither UDP nor MQTT makes no sense, aborting!\n";
	    return -1;
	}
	while(true) {
	    //update and send data
	    std::cout << "INFO: generating data\n";
	    std::string json = participant->createJsonObj();
	    if(useUDP) {
		std::cout << "INFO: sending Data via UDP\n";
		std::chrono::steady_clock::time_point end, begin = std::chrono::steady_clock::now();
		participant->UDPsendData(json);
		end = std::chrono::steady_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " [µs] needed for UDP send operation\n";
	    }
	    if(useMQTT) {
		std::cout << "INFO: sending Data via MQTT\n";
		std::chrono::steady_clock::time_point end, begin = std::chrono::steady_clock::now();
		participant->MQTTsendData(json);
                end = std::chrono::steady_clock::now();
                std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " [µs] needed for MQTT send operation\n";
	    }
	    //wait as specified
	    sleep(sleepDur);
	}
    } catch (char const *c) {
        std::cout << "ERROR " << std::endl;
        std::cout << c << std::endl;
    }
    return 0;
}
