#include "util.h"

Util::Util() {
	this->f = new FileInterface();
}

Util::~Util() {
	delete this->f;
}

std::string Util::getCurrentTimestamp() {
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

unsigned Util::cstrtoui(char* sval) {
	if(sval == NULL) {
		return 0;
	}
	unsigned ival = 0;
	for(unsigned i = 0; i < strlen(sval); i++) {
		ival *= 10;
		ival += (*(sval+i))-48;
	}
	return ival;
}
bool Util::logError(std::string file, std::string line, std::string message) {
	char* path = getenv("POWERCENTER_LOG_DIRECTORY");
	if(path == NULL) {
		return false;
	}
	FileInterface loc_f = FileInterface(std::string(path), true);
	time_t rawtime;
	struct tm* ptm;
	time(&rawtime);
	ptm = gmtime(&rawtime);
	char* timestring = NULL;
	strftime(timestring, 80, "%F %T [%z]", ptm);
	std::string logstring = std::string(timestring)+"\t"+file+"\t"+line+"\t"+message+"\n"; //Shall have the following format: Timestamp\tFileName\tLine\tMessage (Containing eventual IP/Port information)\n
	loc_f.fwrite(logstring);
	return true;
}
FileInterface* Util::getFI() {
	return this->f;
}
std::string Util::generateHTTP_Response(unsigned statuscode, std::string BodyMessage, std::string ContentType) {

    std::string message, statuscodeDescription;
    unsigned BM_length = 0;
    if(statuscode == 200) {
            statuscodeDescription = "OK";
            BM_length = BodyMessage.length();
    } else if(statuscode == 400) {
            statuscodeDescription = "Bad Request";
    } else if(statuscode == 403) {
            statuscodeDescription = "Forbidden";
    } else if(statuscode == 404) {
            statuscodeDescription = "Not Found";
    } else if(statuscode == 405) {
            statuscodeDescription = "Method Not Allowed";
    } else if(statuscode == 500) {
            statuscodeDescription = "Internal Server Error";
    } else {
            statuscodeDescription = "Unknown Statuscode";
    }
    message = "HTTP/1.1 " + std::to_string(statuscode) + " " + statuscodeDescription + "\r\n";
    message += "Server: CustomServer (Dockerized)\r\n";
    message += "Content-Length: "+ std::to_string(BM_length)+"\r\n";
	message += "Content-Type: "+ContentType+"; charset=utf-8\r\n";
    message += "connection: close\r\n";
    message += "\r\n";

    if(statuscode == 200) {
        message += BodyMessage;
    }

    return message;
}

bool Util::sendHTTP_Response(int fsock, struct sockaddr_in clientAddr, unsigned ResponseCode, std::string UserAgent, std::string BodyMessage, std::string ContentType) {

    std::string reply = this->generateHTTP_Response(ResponseCode, BodyMessage, ContentType);
	bool sendBytes = send(fsock, reply.c_str(), strlen(reply.c_str()), 0);
	if(sendBytes < 0) {
		std::cerr << "ERROR: Sending HTTP-Reply failed!" << std::endl;
	}
	this->logHTTP_Reply(clientAddr, UserAgent, ResponseCode, sendBytes);
	return sendBytes;
}

bool Util::logHTTP_Reply(struct sockaddr_in clientAddr, std::string UserAgent, unsigned ResponseCode, int numBytes) {

	char* path = getenv("POWERCENTER_LOG_DIRECTORY");
    if(path == NULL) {
            return false;
    }
    FileInterface loc_f = FileInterface(std::string(path)+"access.log", true);
    time_t rawtime;
    struct tm* ptm;
    time(&rawtime);
    ptm = gmtime(&rawtime);
    char timestring[80] = {0};
    strftime(timestring, 80, "%F %T [%z]", ptm);
	//Shall have the following format: Timestamp\tResponseCode\tNumBytes\tClientIP\tClientUA\n
	std::string lTime(timestring);
	std::string lreCode = std::to_string(ResponseCode);
	std::string lnumB = std::to_string(numBytes);
    std::string logstring = lTime+"\t"+lreCode+"\t"+lnumB+"\t"+std::string(inet_ntoa(clientAddr.sin_addr))+"\t"+UserAgent+"\n";
    loc_f.fwrite(logstring);
	std::cout << logstring;
	return true;
}

int Util::saveUDP_Data(char* data) {
	char* DDir = getenv("POWERCENTER_DATA_DIRECTORY");

	if(DDir == NULL) {
		std::cerr << "ERROR: Environment Variable Data Directory not set\n";
		return FILE_ERROR;
	}

    if(*data != '{' || *(data + strlen(data) -1) != '}') {
            std::cerr << "ERROR: did not receive JSON-Formatted data!" << std::endl;
            return FILE_ERROR | UDP_ERROR;
    }
	//evaluate id

    char needle[] = "\"id\": ";
    if(strstr(data, needle) == nullptr) {
            return FILE_ERROR | UDP_ERROR;
    }
    char* spos = strstr(data, needle) + strlen(needle);
    if(strstr(spos, ",") == nullptr) {
            return FILE_ERROR | UDP_ERROR;
    }
    char* epos = strstr(spos, ",");
    const size_t len = (epos - spos)+1;
    char id[len] = {0};
	strncpy(id, spos, len-1);

	//evaluate Package number
	needle[1] = 'n';
	needle[2] = 'r';
	if(strstr(data, needle) == nullptr) {
	    return FILE_ERROR | UDP_ERROR;
    }
    spos = strstr(data, needle) + strlen(needle);
    if(strstr(spos, ",") == nullptr) {
        return FILE_ERROR | UDP_ERROR;
    }
    epos = strstr(spos, ",");
    const size_t PNrlen = (epos - spos)+1;
	char PNr[PNrlen] = {0};
        strncpy(PNr, spos, PNrlen-1);
	
	//actually save the data
    std::cout << "INFO: Saving data for participant #" << id << std::endl;
    std::string datastring(data), fPath = std::string(DDir);
	fPath += "/" + std::string(id);

    FileInterface* df;
    if(std::string(id).find("..") != std::string::npos) {
        std::cerr << "ERROR: tried to save Data above the folder for data!" << std::endl;
        return FILE_ERROR;
    }

    try {
        FileInterface(fPath, false);
    } catch (std::exception& e) {
        fopen(fPath.c_str(), "w+");
    std::string idStr = (std::string(id)+"\n");

    FileInterface* fl = new FileInterface(std::string(DDir) + "/list", true);
    fl->fwrite(idStr);

    delete fl;
    std::cout << "INFO: created new file for participant #" << id << std::endl;
    }

    try {
        df = new FileInterface(fPath, false);
        unsigned FPnr = 0;
        std::vector<std::string> cpack = df->flines(',');
        for(int i = 0; i< cpack.size(); i++) {
            if(cpack[i].find("\"nr\": ")!= std::string::npos) {
                FPnr = stoul(cpack[i].substr(cpack[i].find(":")+1));
                break;
            }
        }
        if(FPnr >= this->Util::cstrtoui(PNr)) {
            std::cerr << "ERROR: Received duplicate or old package!\n";
	    delete df;
	    return FILE_ERROR;
        } else {
            df->fwrite(datastring);
        }
	strcpy(data, id);
        delete df;
    } catch(std::exception &e) {
         std::cerr << "ERROR: Unable to open REST-file; Error was:" << std::endl << e.what() << std::endl;
         return FILE_ERROR;
    }

    try {
        FileInterface* f = new FileInterface(std::string(DDir) + "/history", true);
        if(f->fread() != "") {
                datastring = ", " + datastring;
        }
        f->fwrite(datastring);
        delete f;
        return NO_ERROR;
    } catch(std::exception &e) {
        std::cerr << "ERROR: Unable to open file!\n";
        return FILE_ERROR;
    }
}
