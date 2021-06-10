#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cstring>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "fileinterface.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>

#define BUFFERSIZE 1024
#define BUFFERSIZE_TCP 4096
#define NO_ERROR 0
#define UDP_ERROR 1
#define TCP_ERROR 2
#define HTTP_ERROR 4
#define FILE_ERROR 8
#define FORK_ERROR 16

class Util {
public:
	Util();
	~Util();
	unsigned cstrtoui(char* sval);
	std::string getCurrentTimestamp();
	bool logError(std::string file, std::string line, std::string message);
	FileInterface* getFI();
	std::string generateHTTP_Response(unsigned statuscode, std::string BodyMessage = "", std::string ContentType="application/json");
	bool sendHTTP_Response(int fsock, struct sockaddr_in clientAddr, unsigned ResponseCode, std::string UserAgent = "", std::string BodyMessage = "", std::string ContentType = "application/json");
	bool logHTTP_Reply(struct sockaddr_in clientAddr, std::string UserAgent, unsigned ResponseCode, int numBytes);
	int saveUDP_Data(char* data);
	std::string getIP(unsigned id) {
		return this->lastSeen[id];
	}
	void updateLastSeen(unsigned id, std::string ip) {
		this->lastSeen[id] = ip;
	}
private:
	FileInterface* f;
	std::map<unsigned, std::string> lastSeen;
};

#endif // UTIL_H
