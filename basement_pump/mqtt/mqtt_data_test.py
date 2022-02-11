# mqq data
# sets up a data channel
# receives set and get messages
"""
sudo pip install paho-mqtt
or
git clone https://github.com/eclipse/paho.mqtt.python.git

cd paho.mqtt.python

python setup.py install
"""


import paho.mqtt.publish as mqtt
import json

MQTT_SERVER = "127.0.0.1"
MQTT_PATH1 = "data_channel"
#MQTT_PATH2 = "LCD_channel"

data={"cmd":"set","grp":"lcd","line1":"this is line1"}
mqtt.single(MQTT_PATH1, data,hostname=MQTT_SERVER)

## The callback for when the client receives a CONNACK response from the server.
#def on_connect(client, userdata, flags, rc):
#    print("Connected with result code "+str(rc))

#    # Subscribing in on_connect() means that if we lose the connection and
#    # reconnect then subscriptions will be renewed.
#    client.subscribe(MQTT_PATH1)
#    #client.subscribe(MQTT_PATH2)

# The callback for when a PUBLISH message is received from the server.
#def on_message(client, userdata, msg):
#    print(msg.topic+" ...->"+str(msg.payload))
#    try: 
#        mq = json.loads(msq.payload)
#        print mq.keys()
#    except:
#        print " json load fail"
        
#    # more callbacks, etc

#client = mqtt.Client()
#client.on_connect = on_connect
#client.on_message = on_message
#client.connect(MQTT_SERVER, 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.

# Other loop*() functions are available that give a threaded interface and a
# manual interface.
#client.loop_forever()

