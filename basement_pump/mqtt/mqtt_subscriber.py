"""
https://tutorials-raspberrypi.com/raspberry-pi-mqtt-broker-client-wireless-communication/
There are several applications that can be used to send and receive through MQTT, but the simplest on the Raspberry Pi is probably Mosquitto. 
We will install this on the Raspberry Pi first:

sudo apt-get install -y mosquitto mosquitto-clients

mosquitto_sub -h localhost -v -t test_channel

mosquitto_pub -h localhost -t test_channel -m "Hello Raspberry Pi"







sudo pip install paho-mqtt
or
git clone https://github.com/eclipse/paho.mqtt.python.git

cd paho.mqtt.python

python setup.py install




"""
import paho.mqtt.client as mqtt
import json

MQTT_SERVER = "0.0.0.0"
MQTT_PATH1 = "test_channel"
MQTT_PATH2 = "LCD_channel"
MQTT_PATH3 = "data_channel"

 

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(MQTT_PATH1)
    client.subscribe(MQTT_PATH2)
    client.subscribe(MQTT_PATH3)

 # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" ...->"+str(msg.payload))
    if msg.topic == 'data_channel':
        print "got data"
        jq=json.loads(msg.payload)
        print "cmd = " + jq["cmd"]
        print "grp = " + jq["grp"]
        print "var = " + jq["var"]
        
    # more callbacks, etc

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_SERVER, 1883, 60)

 # Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.

# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()

###

#import paho.mqtt.publish as publish

#MQTT_SERVER = "127.0.0.1"
#MQTT_PATH = "test_channel"

#publish.single(MQTT_PATH, "Hello World!", hostname=MQTT_SERVER)
###
