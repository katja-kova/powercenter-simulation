#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include "fileinterface.h"
#include "util.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "powercenter.pb.h"
#include "powercenter.grpc.pb.h"
#include "rpcclient.h"
#include "rpcshistory.h"

#include "myMosq.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Server;
using grpc::ServerContext;
using grpc::ServerBuilder;
using grpc::Status;

using powercenter::History;
using powercenter::ReqHistoryMessage;
using powercenter::ReplyHistoryMessage;

using powercenter::DataRequest;
using powercenter::DataResponse;

void loadData(std::string id, std::string value){
    std::string ip;
    if (id == "1") {
        ip = "172.20.0.10:"+std::string(getenv("POWERCENTER_RPC_PORT"));
    } else if(id == "2") {
        ip = "172.20.0.20:"+std::string(getenv("POWERCENTER_RPC_PORT"));
    } else if(id == "3") {
        ip = "172.20.0.30:"+std::string(getenv("POWERCENTER_RPC_PORT"));
    } else {
        ip = "172.20.0.40:"+std::string(getenv("POWERCENTER_RPC_PORT"));
    }
    RPCclient rpcclient(grpc::CreateChannel(ip, grpc::InsecureChannelCredentials()));
    rpcclient.getData(id, value);
}

int handleTCP(Util* util) {
	unsigned port = util->cstrtoui(getenv("POWERCENTER_TCP_PORT"));
	if(port == 0/* || data_dir == NULL*/) {
		return TCP_ERROR | FILE_ERROR;
	}
	int sock, fsock, pid, readstatus;
	socklen_t client_addr_len;
	struct sockaddr_in server_addr, client_addr;
	char buf[BUFFERSIZE];
	//set server_addr

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if( (sock=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Cannot create socket!\n";
		return TCP_ERROR;
	}
	if(bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Cannot bind socket!\n";
		return TCP_ERROR;
	}
	listen(sock, 5);
	while(true) {
		client_addr_len = sizeof(client_addr);
		fsock = accept(sock, (struct sockaddr*)&client_addr, &client_addr_len);
		if(fsock < 0) {
			std::cerr << "Cannot accept connections!\n";
		}
		if((pid = fork())<0) {
			std::cerr << "Cannot fork!\n";
			return FORK_ERROR | TCP_ERROR;
		} else if(pid==0) {
			//child process: close listening socket
			close(sock);
			//read from fsock
			int bufsize_left = BUFFERSIZE_TCP, used_space=0;
			do {
				readstatus = recvfrom(fsock, buf, bufsize_left, 0, (struct sockaddr*)&client_addr, &client_addr_len);
				if(readstatus < 0) {
					std::cerr << "read failed!\n";
					exit(TCP_ERROR);
				}
				bufsize_left -= readstatus;
				used_space += readstatus;
				if(buf[used_space -4] == '\r' && buf[used_space -3] == '\n' && buf[used_space -2] == '\r' && buf[used_space -1] == '\n') {
					break;
				}
			} while(readstatus != 0 && bufsize_left > 0);
			if(bufsize_left <= 0) {
				std::cerr << "Buffer overflow!\n";
				exit(TCP_ERROR);
			}
			//everything with the receiving of data went well!
			//call for HTTP interpretion
		        std::vector<std::string> lines;
		        std::string l;
		        unsigned c_off = 0;
		        std::string reply;
		        while(c_off < strlen(buf)-1) {
        		        if(*(c_off + buf) == '\r' && *(c_off + buf + 1) == '\n') {
		                        lines.push_back(l);
		                        l="";
		                        c_off++;
		                } else {
					char c = *(buf + c_off);
		                        l.append(&c);
		                }
		                c_off++;
		        }
			std::cout << "successfully received TCP Request\n";
		        if(lines.size() < 3 || lines.at(lines.size()-1) != "") {
				//some badly formatted request, dismiss it! (Error 400 Bad Request)
				util->sendHTTP_Response(fsock, client_addr, 400);
		                exit(HTTP_ERROR);
		        }
		        size_t poss = 0, pose=0;
		        std::string res, clientIP, userAgent = "";
		        if((pose = lines[0].find(" ")) == std::string::npos) {
				//some badly formatted request, dismiss it! (Error 400 Bad Request)
				util->sendHTTP_Response(fsock, client_addr, 400);
		                exit(HTTP_ERROR);
		        }
		        if(lines[0].substr(0,pose) != "GET") {
		                //no HTTP-GET request, dismiss it! (Error 405 Method not allowed)
		                util->sendHTTP_Response(fsock, client_addr, 405);
				exit(HTTP_ERROR);
		        }
			poss = pose+1;
			if((pose = lines[0].find(" ", poss)) == std::string::npos) {
                                //some badly formatted request, dismiss it! (Error 400 Bad Request)
                                util->sendHTTP_Response(fsock, client_addr, 400);
                                exit(HTTP_ERROR);
                        }
			std::string rname = lines[0].substr(poss, pose-poss);
			if(rname[0] != '/' || rname.find("..") != std::string::npos) {
				//stopping attempted directory-traversal attack
				util->sendHTTP_Response(fsock, client_addr, 403);
                                exit(HTTP_ERROR);
			}
			std::string fCont;
			std::string ContType = "application/JSON";
			//get UA
			for(int i =1; i<lines.size()-1; i++) {
				if(lines[i].find("User-Agent") != std::string::npos) {
					userAgent = lines[i].substr(lines[i].find(": ")+2);
					break;
				}
			}
			size_t splitPos = 0;
			if(rname == "/") {
				//Show the "index-Page"
				fCont = "<html><h1>Welcome to Powercenter!</h1></html>";
				ContType = "text/html";
			} else if (rname.find("/set/") == 0 && (splitPos = rname.find("/", 6)) != std::string::npos) {
				std::string id = rname.substr(5, splitPos-5);
				std::string value = rname.substr(splitPos+1);
                fCont = "<html><h2>RPC Call to: </h2><br><h1>ID: </h1> " + std::string(id) + "<h1>Value: </h1>" + std::string(value) + "</html>";
                ContType = "text/html";
				loadData(id, value);
			} else {
				try {
		                	char *DDir = getenv("POWERCENTER_DATA_DIRECTORY");				
        		        	if (DDir == NULL) {
                        			util->sendHTTP_Response(fsock, client_addr, 500, userAgent);
			                        exit(HTTP_ERROR);
			                }
		                    	util->getFI()->setFP(std::string(DDir) + rname);
		                    	if (rname == "/history") {
		                        	fCont = "[" + util->getFI()->fread() + "]";
		                    	} else {
		                            	if(rname == "/list") {
		                                	ContType = "text/plain";
		                            	}
		                            	fCont = util->getFI()->fread();
		                        }
				} catch(std::invalid_argument &e) {
					std::cerr << e.what() << std::endl;
		                        //404, the classic
		                        util->sendHTTP_Response(fsock, client_addr, 404, userAgent);
                                        exit(HTTP_ERROR);
		                }
	                }
                	util->sendHTTP_Response(fsock, client_addr, 200, userAgent, fCont, ContType);
                	exit(NO_ERROR);
		} else if(pid > 0) {
			//parent process: close accepting socket
			close(fsock);
		} else {
			exit(FORK_ERROR);
		}
	}
}

int handleUDP(Util* util) {

    //get env data
    unsigned port = util->cstrtoui(getenv ("POWERCENTER_UDP_PORT"));

    if(port == 0) {
        std::cerr << "ERROR: Environment variable for port not set!" << std::endl;
        return UDP_ERROR;
    }

    //define socket and buffers
    int sock, n;
    char buf[BUFFERSIZE]={0};

    struct sockaddr_in server_addr, client_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if((sock = socket(AF_INET, SOCK_DGRAM, 0))<0) {
        std::cerr << "ERROR: Socket creation failed!" << std::endl;
        return UDP_ERROR;
    }

    //bind the socket to IP/Port
    if((bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)))<0) {
        std::cerr << "ERROR: Socket bind failed!" << std::endl;
        return UDP_ERROR;
    }

	while(true) {
	    socklen_t client_addr_len = sizeof(client_addr);
		memset(buf, 0, BUFFERSIZE);
		n = recvfrom(sock, buf, BUFFERSIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
		if(n<0 && buf != "") {
			std::cerr << "ERROR: Transmission failed!" <<std::endl;
			continue;
		}
		std::cout << "INFO: Successfully received UDP-Packet from " << inet_ntoa(client_addr.sin_addr) << std::endl;
        std::cout << "TIMESTAMP: " << util->getCurrentTimestamp() << std::endl;
		if(util->saveUDP_Data(buf) == NO_ERROR) {
			util->updateLastSeen(stoi(std::string(buf)), inet_ntoa(client_addr.sin_addr));
		}
		
	}
}
// */

void handleMqttMessage(struct mosquitto* _mosq, void* _dobj, const struct mosquitto_message* msg) {
	std::cout << "INFO: MQTT-message received!\n";
	std::string buf = mqtt_client::msg2string(msg);
	Util* u = new Util();
	u->saveUDP_Data((char*)buf.c_str());
	std::cerr << "INFO: MQTT-message saved!\n";
}

void RunHistoryServer() {
	char* ge = getenv("POWERCENTER_RPC_PORT");
	if(ge == NULL) {
		std::cerr << "cannot read env-variable: POWERCENTER_RPC_PORT" << std::endl;
		return;
	}
	std::string srv_addr("0.0.0.0:"+std::string(getenv("POWERCENTER_RPC_PORT")));
	rpcHistoryServer* svc = new rpcHistoryServer();
	grpc::EnableDefaultHealthCheckService(true);
	grpc::reflection::InitProtoReflectionServerBuilderPlugin();
	ServerBuilder b;
	b.AddListeningPort(srv_addr, grpc::InsecureServerCredentials());
	b.RegisterService(svc);
	std::unique_ptr<Server> srv(b.BuildAndStart());
	std::cout << "STARTUP: Server started and listening on " << srv_addr << std::endl;
	srv->Wait();
	delete svc;
}

int main(int argc, char *argv[]) {

	//generate common stuff
	Util* util = new Util();
	int Errcoll = 0;

	std::cout << "STARTUP: grpc init\n";
	std::thread T_grpc_History 	= std::thread(RunHistoryServer);
	std::cout << "STARTUP: Websockets init\n";
	std::thread T_TCP 		= std::thread(handleTCP, util);
	std::thread T_UDP		= std::thread(handleUDP, util);
	std::cout << "STARTUP: mqtt init\n";
	mqtt_client* mqtt = mqtt_client::getInstance();
	if(mqtt == nullptr) {
		std::cerr << "ERROR: Failed to initialize mqtt client!\n";
		return 1;
        }
        mqtt->setCallback(handleMqttMessage);
        std::string topic = "data/#";
        mqtt->subscribe(topic.c_str());
	mqtt->initInfiniteLoop();
	T_grpc_History.join();
	T_TCP.join();
	T_UDP.join();
	return 0;
}
