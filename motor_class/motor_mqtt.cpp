#include <string>
#include <iostream>
#include <mosquittopp.h>
//nah not quite yet




class MCMqtt : public mosqpp::mosquittopp
{
public:
	MCMqtt(const char *id, const char *host, int port);
	~MCMqtt();

	void on_connect(int rc);
	void on_message(const struct mosquitto_message *message);
	void on_subcribe(int mid, int qos_count, const int *granted_qos);
};

MCMqtt::MCMqtt(const char *id, const char *host, int port) : mosquittopp(id)
{
  mosqpp::lib_init();			// Initialize libmosquitto
  
  int keepalive = 120; // seconds
  connect(host, port, keepalive);		// Connect to MQTT Broker
}

void MCMqtt::on_connect(int rc)
{
  printf("Connected with code %d. \n", rc);
  
  if (rc == 0)
    {
      subscribe(NULL, "motor/data");
    }
}

void MCMqtt::on_subcribe(int mid, int qos_count, const int *granted_qos)
{
  printf("Subscription succeeded. \n");
}


int main()
{
  int res;
  MCMqtt * mcmqtt;
  mcmqtt = new MCMqtt("Motor Data", "localhost", 1883);
  string mdata ="{\"motor\":\"left\",\"speed\": \"1234\"}";
  
  mcmqtt->publish(NULL, "motor/data", mdata.length(), mdata.c_str());	// Publish the data to MQTT topic "motor/data"

  while (1) {
  	res = mcmqtt->loop();		   // Keep MQTT connection		
	if (res)
		mcmqtt->reconnect();
  }
  return 0;
  
}
