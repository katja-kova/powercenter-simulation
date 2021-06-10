//
// Created by katja on 29.04.21.
//

#include "Participant.h"

void Participant::createConsumer(std::string ip, std::string port, std::string mode, std::string id) {

     while(true) {

         srand (time(NULL));
         nr += 1;

         // CONSUMER
         // possible amounts:
         // active - 1900-2200 kwH
         // still - 1400-1600 kwH

         if(consmax==0){
             // clear the kw value
             conscurr = 0;
         } else {
             if(conscurr<consmax || consmax==1){
                 conscurr += generateValue(2200, 1900);
             } else {
                 conscurr = 0;
                 std::cout << "INFO: Threshold is reached. Disabling consumer... "<< std::endl;
             }
         }
         // create json object
         std::string data = createJsonObj(nr, id, mode, conscurr);
         // sand data to the server
         UDPsendData(ip, port, data);
         // frequency of packages
         sleep(50);
     }
}

void Participant::createProducer(std::string ip, std::string port, std::string mode, std::string id) {

    while(true) {
        srand (time(NULL));
        nr += 1; // package sequence number
        std::string data;
        /*---------------------
         * defined id's in docker-compose.yml
         1 windplant
         2 nuclearplant
         3 solarplant
         ---------------------*/
	
        if (id == "1") {

            // WINDPLANT
            // possible amounts:
            // windy - 600-800 kwH
            // still - 400-600 kwH
            if(windmax==0){
                // clear the kw value
                windcurr = 0;
            } else {
                if(windcurr<windmax || windmax==1){
                    // generate kw value
                    windcurr += generateValue(800, 600);
                } else {
                    windcurr = 0;
                    std::cout << "INFO: Threshold is reached. Disabling windplant... "<< std::endl;
                }
            }
            // create json string
            data = createJsonObj(nr, id, mode, windcurr);

        } else if (id == "2") {

            // NUCLEARPLANT
            // possible amounts:
            // productive - 24000-27000 kwH
            // still - 22000-24000 kwH

            if(nuclmax==0){
                // clear the kw value
                nuclcurr = 0;
            } else {
                if(nuclcurr<nuclmax || nuclmax==1){
                    // generate kw value
                    nuclcurr += generateValue(27000, 24000);
                } else {
                    nuclcurr = 0;
                    std::cout << "INFO: Threshold is reached. Disabling nuclearplant... "<< std::endl;
                }
            }
            // create json string
            data = createJsonObj(nr, id, mode, nuclcurr);

        } else if (id == "3") {

            // SOLARPLANT
            // possible amounts:
            // sunny - 9000-12000 kwH
            // cloudy - 7000-9000 kwH

            if(solmax==0){
                // clear the kw value
                solcurr = 0;
            } else {
                if(solcurr<solmax || solmax==1){
                    // generate kw value
                    solcurr += generateValue(12000, 9000);
                } else {
                    solcurr = 0;
                    std::cout << "INFO: Threshold is reached. Disabling solarplant... "<< std::endl;
                }
            }
            // create json string
            data = createJsonObj(nr, id, mode, solcurr);
        } else {
            std::cerr << "ERROR: Producer type undefined" << std::endl;
        }

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

void Participant::setValue(std::string id, std::string value) {
    std::cout << "INFO: received id: " << id << ", value: " << value << std::endl;
    if(id == "1"){ // case WINDPLANT
        windmax = std::stoi(value);
    } else if(id == "2") { // case NUCLEARPLANT
        nuclmax = std::stoi(value);
    } else if(id == "3") { // case SOLARPLANT
        solmax = std::stoi(value);
    } else if(id == "4") { // case FARM
        consmax = std::stoi(value);
    } else {
        std::cout << "ERROR: Invalid `id` / `value` received from RPC "<< std::endl;
    }// */

}
int Participant::generateValue(int max, int min) {

    int active = rand()%2;
    int nmax = max-400;
    int nmin = min-400;
    int result = 0;

    if(active){
        result = (rand() % (max + 1 - min) + min);
    } else {
        result = (rand() % (nmax + 1 - nmin) + nmin);
    }
    return result;
}

