#include "myMosq.h"

mqtt_client* mqtt_client::singleton = nullptr;

mqtt_client::mqtt_client(char* _id, char* _host, int _port, int _keepalive) {
	mosquitto_lib_init();
	this->host = _host;
	this->port = _port;
	this->msgId = 1;
	this->QoS = 2;
	this->keepalive = _keepalive;
	this->mosq = mosquitto_new(_id, false, this->mosqdataobj);
	mosquitto_connect(this->mosq, _host, _port, _keepalive);
}

mqtt_client::~mqtt_client() {
	mosquitto_disconnect(this->mosq);
	mosquitto_destroy(this->mosq);
	mosquitto_lib_cleanup();
}
mqtt_client* mqtt_client::getInstance() {
	if(singleton== nullptr) {
		char* id = getenv("MQTT_ID");
		char* host = getenv("MQTT_HOST");
		char* cs_port = getenv("MQTT_PORT");
		char* cs_keepalive = getenv("MQTT_TIMEOUT");
		if(id == NULL || host == NULL || cs_port == NULL || cs_keepalive == NULL) {
			std::cerr << "Missing Environment parameter for MQTT!\n";
			return nullptr;
		}
		int port = atoi(cs_port);
		int keepalive = atoi(cs_keepalive);
		if(port < 1 || keepalive < 0) {
			std::cerr << "Invalid port or keepalive time\n";
		}
		singleton = new mqtt_client(id, host, port, keepalive);
	}
	return singleton;
}

void mqtt_client::publish(const char* topic, const char* msg) {
	std::cout << "Publish to topic '" << topic << "' Message: " << msg << std::endl;
	int chk = mosquitto_publish(this->mosq, &this->msgId, topic, strlen(msg), (void *) msg, this->QoS, true);
	chk |= mosquitto_loop(this->mosq,-1, 1);
	if(chk == MOSQ_ERR_SUCCESS) {
		std::cout << "Publish successful!\n";
	} else {
		std::cerr << "Error while publishing! CheckCode: " << chk << std::endl;
	}
}

void mqtt_client::subscribe(const char* topicPattern) {
	std::cout << "subscribing to topic: " <<  topicPattern << std::endl;
	mosquitto_subscribe(this->mosq, &this->msgId, topicPattern, this->QoS);
}

void mqtt_client::setCallback(void (*fnc)(struct mosquitto* _mosq, void* _dobj, const struct mosquitto_message* msg)) {
	std::cerr << "Setting callback function.\n";
	mosquitto_message_callback_set(this->mosq, fnc);
}

void mqtt_client::msg2chars(char* buf, const struct mosquitto_message* msg) {
        //get message
	int payload_size = MAX_PAYLOAD + 1;
        char cstr[payload_size];
        memset(cstr, 0, payload_size * sizeof(char));

        /* Copy N-1 bytes to ensure always 0 terminated. */
        memcpy(cstr, msg->payload, MAX_PAYLOAD * sizeof(char));

	strcpy(buf, cstr);
        #ifdef DEBUG
            std::cout << buf << std::endl;
        #endif
}
