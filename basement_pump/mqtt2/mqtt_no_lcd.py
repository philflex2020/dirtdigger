"""
https://tutorials-raspberrypi.com/raspberry-pi-mqtt-broker-client-wireless-communication/
There are several applications that can be used to send and receive through MQTT, but the simplest on the Raspberry Pi is probably Mosquitto. 
We will install this on the Raspberry Pi first:

sudo apt-get install -y mosquitto mosquitto-clients

mosquitto_sub -h localhost -v -t test_channel

mosquitto_pub -h localhost -t test_channel -m "Hello Raspberry Pi"
mosquitto_sub -h localhost -v -t "\$SYS/#"

sudo pip install paho-mqtt
or
git clone https://github.com/eclipse/paho.mqtt.python.git

cd paho.mqtt.python

python setup.py install

This version will simply display messages (delivered in json) for the lcd

Teset this with 
mosquitto_pub -h localhost -t LCD_channel -m '{"key":"lcd","line1":"hello line1","line2":"hi line2"}'

"""
#import RPi_I2C_driver

import paho.mqtt.client as mqtt
import json

MQTT_SERVER = "localhost"
#MQTT_PATH1 = "test_channel"
MQTT_PATH2 = "LCD_channel"

#mylcd = RPi_I2C_driver.lcd()

#json.dumps
SW_PATH = "SWITCH_in"
switch = {}


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    print("subscribed to mqtt_path2: ["+ MQTT_PATH2+"]")
    print("subscribed to switch_path: ["+ SW_PATH+"]")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(MQTT_PATH1)
    client.subscribe(MQTT_PATH2)
    client.subscribe(SW_PATH)

 # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global switch
    print(msg.topic+" LCDj->"+str(msg.payload))
    
    jmes = json.loads(msg.payload)
    if jmes['key'] == 'switch':
        switch[jmes['chan']]=jmes['value']
    if jmes['key'] == 'lcd':
        print(" LCD line1->"+ jmes["line1"])
        print(" LCD line2->"+ jmes["line2"])
        #mylcd.backlight(0)
        #mylcd.lcd_display_string(jmes["line1"],1)
        #mylcd.lcd_display_string(jmes["line2"],2)
    # more callbacks, etc

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_SERVER, 1883, 60)
#client.loop_start
 # Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.

# Other loop*() functions are available that give a threaded interface and a
# manual interface.
print("subscriing to mqtt_path2: ["+ MQTT_PATH2 + "]")
print("subscriing to switch_path: ["+ SW_PATH + "]")

client.loop_forever()

###

#import paho.mqtt.publish as publish

#MQTT_SERVER = "127.0.0.1"
#MQTT_PATH = "test_channel"

#publish.single(MQTT_PATH, "Hello World!", hostname=MQTT_SERVER)
###
