# coding=utf-8
# drives the TM1638 display
# PSW 2019
# used /udp/report instead of the udp query

import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import socket
import fcntl
import struct
import psutil
import threading
import requests
from urllib import urlopen

from time import sleep
from rpi_TM1638 import TMBoards
from RPi import GPIO

DISPLAY_PATH = '/display'
BUTS_PATH = '/dbuts'
CONTROL_PATH = '/dcontrol'

MQTT_SERVER = "127.0.0.1"

uclient = 1;

def msgjson(data):
    mjs = {}
    try:
        mjs = json.loads(data)
        print(" got json")
        res = "OK"
    except ValueError:
        print (" not json")
        res = "Error"
        return mjs, data
    return mjs,res

def decodemjs(mjs):
    #myhostname = socket.gethostname()
    #s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    #s.connect(('8.8.8.8', 1))  # connect() for UDP doesn't send packets
    #ipaddr = s.getsockname()[0]
    #ipaddr = socket.gethostbyname_ex(hostname)[2]
    #url = 'http://myip.dnsomatic.com'
    #info = requests.request('GET',url)
    #extaddr = info.text
    ## create the response
    rsp = {}
    rtopic = ""
    if "topic" in mjs.keys():
        rtopic = mjs["topic"]
    rhost = ""
    if "replyto" in mjs.keys():
        rhost = mjs["replyto"]
    rid = 0
    if "id" in mjs.keys():
        rid  = mjs["id"]
    rseq = 100
    if "seq" in mjs.keys():
        rsp["seq"] = mjs["seq"]
    rsp["topic"] = rtopic
    rsp["seq"] = rseq
    rsp["id"] = rid
    #rsp["hostname"] = myhostname
    #rsp["ipaddr"] = ipaddr
    #rsp["extaddr"] = extaddr
    rsp["function"] ="display driver"
    rsp["inputs"] = DISPLAY_PATH
    rsp["outputs"] = BUTS_PATH
    rsp["control"] = CONTROL_PATH
    return rsp,rtopic,rhost

def enhancemjs(mjs):
    #myhostname = socket.gethostname()
    #s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    #s.connect(('8.8.8.8', 1))  # connect() for UDP doesn't send packets
    #ipaddr = s.getsockname()[0]
    #ipaddr = socket.gethostbyname_ex(hostname)[2]
    #url = 'http://myip.dnsomatic.com'
    #info = requests.request('GET',url)
    #extaddr = info.text
    ## create the response
    #rsp = {}
    rtopic = ""
    if "topic" in mjs.keys():
        rtopic = mjs["topic"]
    rhost = ""
    if "replyto" in mjs.keys():
        rhost = mjs["replyto"]
    rid = 0
    if "id" in mjs.keys():
        rid  = mjs["id"]
    rseq = 100
    if "seq" in mjs.keys():
        rsp["seq"] = mjs["seq"]

    mjs["function"] ="display driver"
    mjs["inputs"] = DISPLAY_PATH
    mjs["outputs"] = BUTS_PATH
    mjs["control"] = CONTROL_PATH
    return mjs,rtopic,rhost
    
#run this is a different thread
def setupUdp (uport):
    global uclient

    #uport = 37020
    uclient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    uclient.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    uclient.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    uclient.bind(("",uport))

def udpThread(v, stop):
    global uclient
    data,addr = uclient.recvfrom(1024)
    print ( "received udp message :[%s] from [%s]" % (data,addr))
    mjs, res = msgjson(data)
    print ( "decode udp message :[%s]" % res)
    if res == "OK":
        print res
        rsp = {}
        try:
            rsp,rtopic,rhost = decodemjs(mjs)
            msg = json.dumps(rsp);
            print rtopic + "  ",
            print msg
            
            publish.single(rtopic,msg,hostname=rhost)
        except:
            print "decode failed"

        

# my GPIO settings (two TM1638 boards connected on GPIO19 and GPIO13 for DataIO and Clock; and on GPIO06 and GPIO26 for the STB)
DIO = 19
CLK = 13
#STB = 06, 26
STB = 06

DISPLAY_PATH = '/display'
BUTS_PATH = '/dbuts'
CONTROL_PATH = '/dcontrol'
MQTT_SERVER = "127.0.0.1"

# instanciante my TMboards
TM = TMBoards(DIO, CLK, STB, 0)

TM.clearDisplay()

TM.leds[0] = True       # turn on led 0 (1st led of the 1st board)
TM.leds[6] = True      # turn on led 12 (5th led of the 2nd board, since there is 8 leds per board)

TM.segments[1] = '0'        # display '0' on the display 1 (2nd 7-segment display of the 1st board)
TM.segments[4] = '12.34'     # display '9876' on the 7-segment display number 4,


def switch_send(buts):
    jmsg = json.dumps(buts);
    publish.single(BUTS_PATH, jmsg, hostname=MQTT_SERVER)

def runDsp( msg, jmes):
    if "dsp2" in jmes.keys():
        TM.segments[4] = jmes["dsp2"]     # display '9876' on the 7-segment display number 4,
    if "dsp1" in jmes.keys():
        TM.segments[0] = jmes["dsp1"]
    for l in range(0,7):
        led = "led" + str(l)
        print led
        if led in jmes.keys():
            TM.leds[l] = jmes[led] == 1

okPublish = 0

def triggerPublish():
    global okPublish
    okPublish += 1



def prepareReport(msg, jmes):
    global rmsg
    global rtopic
    global rhost
    
    try:
        print "Enhance 1 >> "
        rsp,rtopic,rhost = enhancemjs(jmes)
        rmsg = json.dumps(rsp);
        print "Enhance >> " + rtopic + "  ",
        print rmsg
            
        triggerPublish()
        #publish.single(rtopic,rmsg,hostname=rhost)
    except:
        print "enhance failed"

    
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(MQTT_PATH1)
    client.subscribe(DISPLAY_PATH)
    client.subscribe(CONTROL_PATH)
    client.subscribe("/udp/report")

 # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" DISPLAY->"+str(msg.payload))
        
    jmes = {}
    try:
        jmes = json.loads(msg.payload)
    except:
        print " not json"
    # we get this from mqtt_udp_mon  which listens to the broadcast   
    if msg.topic == "/udp/report":
        prepareReport(msg,jmes)
    else:
        runDsp(msg,jmes)

        
ticks = 0
buts = {}

def getButs(a):
    idx  = 0
    for x in a:
        print x,
        print " ",
        print x & 1,

        print " ",
        print x & 16

        if x & 1:
            buts[idx] = 1
        else:
            buts[idx] = 0
        if x & 16:
            buts[idx+4] = 1
        else:
            buts[idx+4] = 0
        idx += 1

#stop_thread = False
if __name__ == "__main__":
    global rhost
    global rmsg
    global rtopic
    global stop_thread
    rtopic = ""
    rhost = ""
    rmsg =  {}
    stop_thread = False

    setupUdp (37030)
    t = threading.Thread(target=udpThread, args=(1,lambda: stop_thread))
    t.start()
    
    buts = {}
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, 1883, 60)

    # Blocking call that processes network traffic, dispatches callbacks and
    # handles reconnecting.

    # Other loop*() functions are available that give a threaded interface and a
    # manual interface.
    a=TM.getData(0)
    getButs(a)
    try:
        while True:
            
            client.loop()
            b=TM.getData(0)
            
            if (a != b):
                
                print ("      " + str(ticks))
                ticks = ticks + 1
                print b
                print a
                getButs(b)
                print buts
                switch_send(buts)
                a = b

            if okPublish > 0:
                 okPublish -=1
                 publish.single(rtopic,rmsg,hostname=rhost)
            sleep(0.1)
                 
    except KeyboardInterrupt:
        global stop_thread
        print 'quitting'
        stop_thread = True
        socket.close(uclient)
        t.join()   
   

