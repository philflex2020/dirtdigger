import paho.mqtt.publish as publish

MQTT_SERVER = "127.0.0.1"
#MQTT_SERVER = "pine64-001"
MQTT_PATH = "data_channel"

publish.single(MQTT_PATH, "Hello World!", hostname=MQTT_SERVER)
