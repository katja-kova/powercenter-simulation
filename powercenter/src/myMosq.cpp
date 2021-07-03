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
//	mosquitto_reconnect_delay_set(this->mosq, 2, 10, false);
}

mqtt_client::~mqtt_client() {
	mosquitto_disconnect(this->mosq);
	mosquitto_destroy(this->mosq);
	mosquitto_lib_cleanup();
}
mqtt_client* mqtt_client::getInstance() {
	if(singleton== nullptr) {
		char* id = getenv("POWERCENTER_MQTT_ID");
		char* host = getenv("POWERCENTER_MQTT_HOST");
		char* cs_port = getenv("POWERCENTER_MQTT_PORT");
		char* cs_keepalive = getenv("POWERCENTER_MQTT_TIMEOUT");
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

void mqtt_client::publish(char* topic, char* msg) {
	std::cout << "INFO: Publish to topic '" << topic << "'@'" << this->host << "' Message: " << msg << std::endl;
	int chk = mosquitto_publish(this->mosq, &this->msgId, topic, strlen(msg), (void *) msg, this->QoS, true);
	chk |= mosquitto_loop(this->mosq, -1, 1);
	if(chk == MOSQ_ERR_SUCCESS) {
                std::cout << "INFO: Publish successful!\n";
        } else if(chk == MOSQ_ERR_CONN_LOST || chk == MOSQ_ERR_NO_CONN){
		std::cerr << "WARN: Connection lost, Reconnecting\n";
		if(chk = mosquitto_reconnect(this->mosq) == MOSQ_ERR_SUCCESS) {
			std::cout << "INFO: Reconnecting attempt successful!\n";
		} else {
			std::cerr << "ERROR: Reconnnecting failed: " << mosquitto_strerror(chk) << std::endl;
		}
	} else {
                std::cerr << "ERROR: Publishing failed! Error description: " << mosquitto_strerror(chk) << std::endl;
        }
}

void mqtt_client::subscribe(const char* topicPattern) {
	std::cout << "INFO: Subscribing to topic: " <<  topicPattern << std::endl;
	int chk = mosquitto_subscribe(this->mosq, &this->msgId, topicPattern, this->QoS);
	if(chk == MOSQ_ERR_SUCCESS) {
		std::cout << "INFO: Subscribing successful!\n";
	} else {
		std::cout << "ERROR: Subscribing failed! Error description: "<< mosquitto_strerror(chk) << std::endl;
	}
}

void mqtt_client::setCallback(void (*fnc)(struct mosquitto* _mosq, void* _dobj, const struct mosquitto_message* msg)) {
	std::cout << "Setting callback function.\n";
	mosquitto_message_callback_set(this->mosq, fnc);
}

void mqtt_client::initInfiniteLoop() {
	int chk = mosquitto_loop_forever(this->mosq, -1, 1);
	if(chk != MOSQ_ERR_SUCCESS) {
		std::cerr << "ERROR: mosquitto loop forever returned, because " << mosquitto_strerror(chk) << std::endl;
	} else {
		std::cout << "INFO: Exited infinite loop normally ?!?";
	}
}

std::string mqtt_client::msg2string(const struct mosquitto_message* msg) {
        //get message
        //
        return std::string((char*)(msg->payload));
 /*       std::cerr << "MQTT-MSG: " << ((char*)(msg->payload)) << std::endl;
	const unsigned msglen = strlen((char*)(msg->payload));
	std::cerr << "Len: " << msglen << std::endl;
	char cstr[msglen+1] = {0};
	std::cerr << "cstr init\n";
//	strncpy(buf, (char*) msg->payload, strlen((char*)(msg->payload)));
	memcpy(cstr, msg->payload, msglen);
	std::cerr << "memcpy cstr\n";
	strcpy(buf, cstr);
	/*int payload_size = MAX_PAYLOAD + 1;
        char cstr[payload_size];
        memset(cstr, 0, payload_size * sizeof(char));*/

        /* Copy N-1 bytes to ensure always 0 terminated. */
/*        memcpy(cstr, msg->payload, MAX_PAYLOAD * sizeof(char));

	strcpy(buf, cstr);// */
//        #ifdef DEBUG
//	strcpy(buf, "");        
//            std::cout << buf << std::endl;
//        #endif
}
