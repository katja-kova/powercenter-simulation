//
// Created by katja on 29.04.21.
//

#include "Participant.h"
/*
public:
    Participant();
    std::string getHostIP();
    static void setValue(std::string id, std::string value);
    int generateValue(int max, int min);
    void createParticipant (std::string ip, std::string port, std::string mode, std::string id);

private:
    int kwCurrent;
    int kwEnvMax;
    int kwEnvMin;
    int kwSetMax;
    int kwFluct;
    int sendLimited;
    int MsgNr;
    std::string mode, id, ip, port;
    mqtt_client* mqtt;

    static std::string getCurrentTimestamp();
    std::string createJsonObj(int nr, const std::string id, const std::string mode, int kw);
    int UDPsendData(const std::string &ip, const std::string &port, std::string data);
    int MQTTSendData();
};
// */
Participant::Participant() {
    // read variables from docker-compose.yml
    // and initialize participant with it   
    std::cerr << "STARTUP: Initializing participant\n";
    if(getenv("PARTICIPANT_MODE") == NULL || getenv("PARTICIPANT_ID")==NULL || getenv("PARTICIPANT_DES_IP")==NULL || getenv("PARTICIPANT_DES_PORT")==NULL) {
	std::cerr << "ERROR: Missing some basic environment variable!\n";
	exit(-1);
    }
    this->mode = std::string(getenv("PARTICIPANT_MODE"));
    this->id = std::string(getenv("PARTICIPANT_ID"));
    this->ip = std::string(getenv("PARTICIPANT_DES_IP"));
    this->port = std::string(getenv("PARTICIPANT_DES_PORT"));
    std::cerr << "STARTUP: basic variables set!\n";
    if(getenv("PARTICIPANT_KW_MAX") == NULL || getenv("PARTICIPANT_KW_MIN")==NULL || getenv("PARTICIPANT_KW_FLUCT")==NULL) {
        std::cerr << "ERROR: Missing some data environment variable!\n";
        exit(-1);
    }
    this->kwEnvMax = atoi(getenv("PARTICIPANT_KW_MAX"));
    this->kwEnvMin = atoi(getenv("PARTICIPANT_KW_MIN"));
    this->kwCurrent = this->kwEnvMin - (this->kwEnvMin-this->kwEnvMax)/2;
    this->kwSetMax = this->kwEnvMax;
    this->kwFluct = atof(getenv("PARTICIPANT_KW_FLUCT"));
    //this->sendLimited = (getenv("PARTICIPANT_LIMIT_MESSAGE_NUMBER") != NULL)? atoi(getenv("PARTICIPANT_LIMIT_MESSAGE_NUMBER")): -1;
    this->MsgNr = 1;
    std::cerr << "STARTUP: data variables set!\n";
    this->mqtt = mqtt_client::getInstance();
    std::cerr << "STARTUP: mqtt initialized!\n";
}

int Participant::UDPsendData(std::string data) {

    int fileDiscriptorID, remoteConnection;
    struct sockaddr_in remoteServAddr, cliAddr;
    struct hostent *h;

    try {

        h = gethostbyname(this->ip.c_str());// ip should be a hostname from docker

        if (h == NULL) {
            throw "ERROR: unknown host";
        }

        std::cout << "INFO: creating connection to server: IP: "
                  << inet_ntoa(*(struct in_addr *) h->h_addr_list[0]) << " , PORT: " << this->port << std::endl;

        remoteServAddr.sin_family = AF_INET;

        memcpy((char *) &remoteServAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);

        remoteServAddr.sin_port = htons(atoi(this->port.c_str()));

        fileDiscriptorID = socket(AF_INET, SOCK_DGRAM, 0);

        if (fileDiscriptorID < 0) {
            throw "ERROR: socket reservation failed: " + (std::string) strerror(errno);
        }

        std::cout << "INFO: sending data: " << data << std::endl;
        remoteConnection = sendto(fileDiscriptorID, data.c_str(), strlen(data.c_str()) + 1, 0,
                                  (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr)); //length of ip address

        std::cout << "TIMESTAMP: " << getCurrentTimestamp() << " package nr: "<< this->MsgNr << std::endl;

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
    return 0;
}
int Participant::MQTTsendData(std::string data) {
    this->mqtt->publish(std::string("orig_data/"+this->id).c_str(), data.c_str());
    return 0;
}

std::string Participant::getCurrentTimestamp() {

    using namespace std::chrono;

    // get current time
    auto now = system_clock::now();

    // get number of milliseconds for the current second
    // (remainder after division into seconds)
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = system_clock::to_time_t(now);

    // convert to broken time
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;

    oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

std::string Participant::getHostIP() {
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

std::string Participant::createJsonObj(){
    //update data
    if(this->kwSetMax == 0) {
	kwCurrent = 0;
    } else {
	kwCurrent = this->generateValue();
    }
    //generate response
    std::stringstream ss;
    std::string data;

    ss << "{\"nr\": " << std::to_string(this->MsgNr++) << "," << "\"id\": " << this->id << "," << "\"mode\": \"" << this->mode << "\"," << "\"value\": " << std::to_string(kwCurrent) << ", \"ip\": \""<< this->getHostIP() << "\"}";

    data = ss.str();

    return data;
}

void Participant::setValue(std::string id, std::string value) {
    std::cout << "INFO: received id: " << id << ", value: " << value << std::endl;
    if(id != this->id) {
	std::cerr << "ERROR: received wrong id!\n";
    } else {
	if(stoi(value) == 1) {
	    this->kwSetMax = this->kwEnvMax;
	} else {
	    this->kwSetMax = stoi(value);
	}
    }
}
int Participant::generateValue() {
    int max = (this->kwEnvMax > this->kwSetMax)?this->kwSetMax:this->kwEnvMax;
    int min = this->kwEnvMin;
    int diff = max - min;
    int trend = round((((double)(rand()%11)-5.0)/13)*this->kwFluct*(double)diff);
    int result = this->kwCurrent + trend;
    if(result > max) {
	result = max;
    } else if(result < min) {
	result = min;
    }
    return result;
}

