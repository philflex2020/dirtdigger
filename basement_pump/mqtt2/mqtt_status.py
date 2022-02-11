"""
receive an mqtt packet and create a log of payload size source and time
mosquitto_pub -t /time/test -m '{"msg":"hi there", "src":"foo","ltime":"123.456"}'

When we get a status request send our status data to the host specified in the request
status includes:
     hostname
     ip address
     uptime
     etc...

"""
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import socket
import fcntl
import struct
#import psutil
#import RPi.GPIO as GPIO
import time
import argparse
import threading 
import math
import datetime
import time

MQTT_SERVER = "localhost"
MQTT_CHAN = "/status/data"
MQTT_HOST = "/status/req"

basetime = time.time()
host = socket.gethostname()
hosts = {}
send_hosts={}


class Host:
    def __init__(self, name):
        self.name = name
        self.min_time = 0.0
        self.max_time = 0.0
        self.sum_time = 0.0
        self.time_count = 0.0

    def setTime(self, htime):
        if self.time_count < 1.0:
            self.min_time = htime
            self.max_time = htime
            self.sum_time = htime
            self.time_count = 1.0
        else:
            if self.min_time > htime:
                self.min_time = htime
            if self.max_time < htime:
                self.max_time = htime
            self.sum_time += htime
            self.time_count += 1.0

    def maxTime(self):
        return self.max_time
    def minTime(self):
        return self.min_time
    
    def avgTime(self):
        if self.time_count > 0.0:
            val = self.sum_time/self.time_count
        else:
            val = 0.0
        return  val

def getLtime():
    return time.time()-basetime

def getDtime(btime):
    return time.time()-btime

#[106.913064957] len[62]  src [pine64-003]  ltime [1084.75315309]
def getBtime(mstr):
    if mstr[0] == '[':
        midx = mstr.find("]")
        mnum = mstr[1:midx]
        return float(mnum)
    return 0.0

def getMtime(mstr):
    midx = mstr.find("ltime [")
    midx += len("ltime [")
    mstr = mstr[midx:]
    midx = mstr.find("]")
    mnum = mstr[:midx]
    return float(mnum)
    

def getMhost(mstr):
    midx = mstr.find("src [")
    midx += len("src [")
    mstr = mstr[midx:]
    midx = mstr.find("]")
    mnum = mstr[:midx]
    return mnum

def setHtime(host, htime):
    global hosts

    if host not in hosts.keys():
        print (" adding new host " + host )
        hosts[host]= Host(host)
    hosts[host].setTime(htime)
    
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(MQTT_CHAN)
    print("Subscribed to " + MQTT_CHAN)

    client.subscribe(MQTT_HOST)
    print("Subscribed to " + MQTT_HOST)

    # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global send_hosts
    global hosts
    if msg.topic == MQTT_CHAN:
        mdata = {}
        mdata["src"] = ""
        mdata["ltime"] = ""
        mdata["size"] = ""
        #print(msg.topic+"->"+str(msg.payload))
        #print(str(getLtime()) + " -> "+str(len(str(msg.payload))))
        jmes = json.loads(msg.payload)
        for k in jmes.keys():
            mdata[k] = jmes[k]
        mstr ="["+str(getLtime()) + "] len["+str(len(str(msg.payload)))+"]"
        mstr +="  src ["+ mdata["src"] + "]"
        mstr +="  ltime [" + mdata["ltime"] + "]"
        print mstr
        mtime = getBtime(mstr)
        mhost = getMhost(mstr)
        htime = getMtime(mstr)
        #print mtime
        print mhost
        #print htime
        setHtime(mhost, getLtime() -htime)
        
    if msg.topic == MQTT_HOST:
        print(msg.topic+"->"+str(msg.payload))
        nhost = msg.payload
        if nhost not in send_hosts.keys():
            send_hosts[nhost]=nhost
            


if __name__ == "__main__":

    global btime
    global send_hosts
    global hosts
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
            #client.loop_forever()
            client.loop()
            if getDtime(btime) > 2.0:
                btime = time.time()
                bstr = str(getLtime())
                msg = '{"msg":"hi there", "src":"'+host+'","ltime":"'+bstr+'"}'
                #print msg
                for nhost in send_hosts.keys():
                    #publish.single(MQTT_CHAN,msg)
                    publish.single(MQTT_CHAN,msg,hostname=nhost)

        except KeyboardInterrupt:

            print 'quitting'
            done = False
    for host in hosts.keys():
        print (host + " avg :" + str(hosts[host].avgTime())),
        print ("max :" + str(hosts[host].maxTime())),
        print ("min :" + str(hosts[host].minTime())),
        print
                

        
