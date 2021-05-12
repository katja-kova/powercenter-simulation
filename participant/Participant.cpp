//
// Created by katja on 29.04.21.
//

#include "Participant.h"

int main(int argc, char *argv[]) {
    std::string mode, ip, port, id;

    Participant *participant = new Participant();

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

void Participant::createConsumer(std::string ip, std::string port, std::string mode, std::string id) {

     while(true) {

         srand (time(NULL));
         int kw = 0;
         nr += 1;

         // CONSUMER
         // possible amounts:
         // active - 1900-2200 kwH
         // still - 1400-1600 kwH

         int active = rand()%2;

         if(active){
             kw = (rand() % (2200 + 1 - 1900)) + 1900;
         } else {
             kw = (rand() % (1600 + 1 - 1400)) + 1400;
         }

         // create json string
         std::string data = createJsonObj(nr, id, mode, kw);
         // sand data to the server
         UDPsendData(ip, port, data);
         // frequency of packages
         sleep(50);
     }
}

void Participant::createProducer(std::string ip, std::string port, std::string mode, std::string id) {

    while(true) {
        srand (time(NULL));
        int kw = 0;
        nr += 1; // package sequence number

        /*---------------------
         * defined id's in docker-compose.yml
         1 windplant
         2 nuclearplant
         3 solarplant
         ---------------------*/

        if (!id.compare("1")) {

            // WINDPLANT
            // possible amounts:
            // windy - 600-800 kwH
            // still - 400-600 kwH

            int windy = rand()%2;

            if(windy) {
                kw = (rand() % (800 + 1 -600)) + 600;
            } else {
                kw = (rand() % (600 + 1 -400)) + 400;
            }
        } else if (!id.compare("2")) {

            // NUCLEARPLANT
            // possible amounts:
            // productive - 24000-27000 kwH
            // still - 22000-24000 kwH

            int productive = rand()%2;

            if(productive) {
                kw = (rand() % (27000 + 1 - 24000)) + 24000;
            } else {
                kw = (rand() % (24000 + 1 - 22000)) + 22000;
            }
        } else if (!id.compare("3")) {

            // SOLARPLANT
            // possible amounts:
            // sunny - 9000-12000 kwH
            // cloudy - 7000-9000 kwH

            int sunny = rand()%2;

            if(sunny) {
                kw = (rand() % (12000 + 1 - 9000)) + 9000;
            } else {
                kw = (rand() % (9000 + 1 - 7000)) + 7000;
            }
        } else {
            std::cerr << "ERROR: Producer type undefined" << std::endl;
        }

        // create json string
        std::string data = createJsonObj(nr, id, mode, kw);
        // sand data to the server
        UDPsendData(ip, port, data);
        // frequency of packages
        sleep(50);
    }
}

int Participant::UDPsendData(const std::string &ip, const std::string &port, std::string data) {

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

        std::cout << "TIMESTAMP: " << getCurrentTimestamp() << " package nr: "<< nr << std::endl;

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

std::string Participant::createJsonObj(int nr, std::string id, std::string mode, int kw){
    std::stringstream ss;
    std::string data;

    ss << "{\"nr\": " << std::to_string(nr) << "," << "\"id\": " << id << "," << "\"mode\": \"" << mode << "\"," << "\"value\": " << std::to_string(kw) << "}";

    data = ss.str();

    return data;
}
