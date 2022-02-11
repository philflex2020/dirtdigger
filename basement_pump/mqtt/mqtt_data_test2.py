import paho.mqtt.publish as publish

MQTT_SERVER = "127.0.0.1"
MQTT_PATH = "data_channel"
msg = '{"cmd":"set","grp":"LCD","var":"line1","value":"Hello line1"}'
publish.single(MQTT_PATH, msg, hostname=MQTT_SERVER)
