//
// Created by katja on 29.04.21.
//
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
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
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

const std::string ip = "172.20.0.60";
const std::string port = "50000";

std::string getCurrentTimestamp() {
    char buffer[64] = {0};
    time_t rawtime;//format struct
    time(&rawtime);//set rawtime with calender date UTC
    const auto timeinfo = localtime(&rawtime);//get current timezone UTC +2
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string timeStr(buffer);

    return timeStr;
}
void UDPsendData(std::string data) {

    int fileDiscriptorID, remoteConnection;
    struct sockaddr_in remoteServAddr, cliAddr;
    struct hostent *h;

    try {

        h = gethostbyname(ip.c_str());// ip should be a hostname from docker

        if (h == NULL) {
            throw "ERROR: unknown host";
        }

        std::cout << "INFO: creating connection to server: IP: "
                  << inet_ntoa(*(struct in_addr *) h->h_addr_list[0]) << " , PORT: " << port << std::endl;

        remoteServAddr.sin_family = AF_INET;

        memcpy((char *) &remoteServAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);

        remoteServAddr.sin_port = htons(atoi(port.c_str()));

        fileDiscriptorID = socket(AF_INET, SOCK_DGRAM, 0);

        if (fileDiscriptorID < 0) {
            throw "ERROR: socket reservation failed: " + (std::string) strerror(errno);
        }

        std::cout << "INFO: sending data: " << data << std::endl;
        remoteConnection = sendto(fileDiscriptorID, data.c_str(), strlen(data.c_str()) + 1, 0,
                                  (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr)); //length of ip address

        std::cout << "timestamp: " << getCurrentTimestamp() << std::endl;

        if (remoteConnection < 0) {
            close(fileDiscriptorID);
            throw "ERROR: data transfer failed: " + errno;
        }

	    close(fileDiscriptorID);

    } catch (const std::exception &e) {
        std::cout << "ERROR: UDP failed with: " << std::endl;
        std::cout << e.what() << std::endl;
    } catch (char const *c) {
        std::cout << "ERROR: UDP failed with: " << std::endl;
        std::cout << c << std::endl;
    }
}


std::string getHostIP() {
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;

    // retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));

    // retrieve host information
    host_entry = gethostbyname(hostbuffer);

    // convert from binary to dot notation
    IPbuffer = inet_ntoa(*((struct in_addr *)
            host_entry->h_addr_list[0]));

    return IPbuffer;
}


void readTestData(const char *path) {

    struct dirent *entry;
    DIR *dir = opendir(path);

    while((entry = readdir(dir)) != NULL) {
        if(entry->d_name[0] == 'c'){
            std::string file = std::string(entry->d_name);

            std::ifstream testFile("./cases/"+file);
            std::string content( (std::istreambuf_iterator<char>(testFile) ),
                (std::istreambuf_iterator<char>()    ) );
            UDPsendData(content);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {

    readTestData("./cases");

    return 0;
}