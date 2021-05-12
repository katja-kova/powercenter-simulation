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

#define PARAM_COUNT_MIN 4
#define PARAM_COUNT_MAX 6

static int nr=0;

class Participant {

public:

    std::string getHostIP();
    void createConsumer (std::string ip, std::string port, std::string mode, std::string id);
    void createProducer (std::string ip, std::string port, std::string mode, std::string id);

private:

    static std::string getCurrentTimestamp();
    std::string createJsonObj(int nr, const std::string id, const std::string mode, int kw);
    int UDPsendData(const std::string &ip, const std::string &port, std::string data);

};

#endif //VS_PARTICIPANT_H
