#ifndef SIMPLECLIENT_MQTT_H
#define SIMPLECLIENT_MQTT_H

#include <mosquitto.h>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <cstdio>

#define MAX_PAYLOAD 50
#define DEFAULT_KEEP_ALIVE 60

class mqtt_client {
private:
	static mqtt_client* singleton;
	mosquitto* mosq;
	char* host;
	int port;
	int keepalive;
	char* topic;
	int msgId;
	int QoS;
	void* mosqdataobj;
	mqtt_client (char* _id, char* _host, int _port, int _keepalive);
	~mqtt_client();
public:
	static mqtt_client* getInstance();
	void publish(char* topic, char* msg);
	void subscribe(const char* topicPattern);
	void setCallback(void (*fnc)(struct mosquitto* _mosq, void* _dobj, const struct mosquitto_message* msg));
	void initInfiniteLoop();
	static std::string msg2string(const struct mosquitto_message* msg);
};

#endif //SIMPLECLIENT_MQTT_H
