"""
receive an mqtt packet and create a log of payload size source and time
mosquitto_pub -t /time/test -m '{"msg":"hi there", "src":"foo","ltime":"123.456"}'

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

basetime = time.time()
host = socket.gethostname()


def getLtime():
    return time.time()-basetime

def getDtime(btime):
    return time.time()-btime
    
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(MQTT_CHAN)
    print("Subscribed to " + MQTT_CHAN)

    # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):

    
    if msg.topic == MQTT_CHAN:
        mdata = {}
        mdata["src"] = ""
        mdata["ltime"] = ""
        mdata["size"] = ""
        print(msg.topic+"->"+str(msg.payload))
        print(str(getLtime()) + " -> "+str(len(str(msg.payload))))
        jmes = json.loads(msg.payload)
        for k in jmes.keys():
            mdata[k] = jmes[k]
        print("["+str(getLtime()) + "] len["+str(len(str(msg.payload)))+"]"),
        print("  src ["+ mdata["src"] + "]"),
        print("  ltime [" + mdata["ltime"] + "]"),
        print
        


if __name__ == "__main__":

    
    print(" starting stuff 1\n")
    print(str(getLtime()) + " -> "+str(len(str(1234))))
    print(" starting stuff 2\n")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, 1883, 60)
    done = True
    btime = time.time()
    while done:
        try:
            global btime
            #client.loop_forever()
            client.loop()
            if getDtime(btime) > 2.0:
                btime = time.time()
                bstr = str(getLtime())
                msg = '{"msg":"hi there", "src":"'+host+'","ltime":"'+bstr+'"}'
                #print msg
                publish.single(MQTT_CHAN,msg)
                publish.single(MQTT_CHAN,msg,hostname="pizero3")

        except KeyboardInterrupt:
            print 'quitting'
            done = False
