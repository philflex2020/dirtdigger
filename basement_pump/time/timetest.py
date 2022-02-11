"""
receive an mqtt packet and create a log of payload size source and time

"""
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import socket
import fcntl
import struct
import psutil
#import RPi.GPIO as GPIO
import time
import argparse
import threading 
import math
import datetime
import time

MQTT_SERVER = "localhost"
MQTT_CHAN = "/time/test"

basetime = time()

def getLtime():
    return time()-basetime
    
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(MQTT_CHAN)
    print("Subscribed to " + MQTT_CHAN)

    # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    mdata = {}
    mdata["src"] = ""
    mdata["ltime"] = ""
    mdata["size"] = ""
    
    if msg.topic == MQTT_CHAN:
        print(msg.topic+"->"+str(msg.payload))
        jmes = json.loads(msg.payload)
        for k in jmes.keys():
            mdata[k] == jmes[k]
            print(str(getLtime) + " -> "+len(str(msg.payload)))

if __name__ == "__main__":

    print(" starting stuff\n")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, 1883, 60)

    try:
        client.loop_forever()

    except KeyboardInterrupt:
        print 'quitting'

        

