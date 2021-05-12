#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>

#define THREAD_COUNT 1000

void requestPage(std::chrono::steady_clock::time_point startTime) {
	//pre measurement stuff
	int s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sAddr;
        sAddr.sin_family = AF_INET;
        sAddr.sin_port = htons(80);
        sAddr.sin_addr.s_addr= htonl(0x7F000001);
	std::string HTTP_REQ = "GET /history HTTP/1.1\r\nHost: 172.20.0.60\r\nUser-Agent: none\r\n\r\n";
	if(connect(s,reinterpret_cast<struct sockaddr*> (&sAddr), sizeof(sAddr)) != 0) {
		std::cerr << "Cannot connect with server!\n";
        }
	
	//ready!
	std::this_thread::sleep_until(startTime);
	//measurement started!
	send(s, &HTTP_REQ[0], int(strlen(&HTTP_REQ[0])), 0);
	while(recv(s, NULL, 2, 0)>0);
	return;
}

int main(int arc, char** argv) {
	std::cout << "starting..."<<std::endl;
	std::vector<std::thread> tvec;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	begin += std::chrono::milliseconds(5000);
	std::chrono::steady_clock::time_point end;
	std::cout << "creating threads:" << std::endl;
	for(int i = 0; i< THREAD_COUNT; i++) {
                tvec.push_back(std::thread(requestPage, begin));
		if(((i+1) % (THREAD_COUNT/10)) == 0) {
			std::cout << 100.0*(double)(i+1)/(double)THREAD_COUNT << "% done" << std::endl;
		}
        }
	std::cout << "Starting Measurement..." << std::endl;
	for(int i = 0; i< THREAD_COUNT; i++) {
		tvec[i].join();
	}
	end = std::chrono::steady_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs] needed for "<< THREAD_COUNT << " requests.\n";
	std::cout << "Average is " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/(double)THREAD_COUNT << "[µs]" << std::endl;
	return 0;
}
