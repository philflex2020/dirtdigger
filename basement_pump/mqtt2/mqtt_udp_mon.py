# coding=utf-8
# PSW 2019
# listens to udp requests and forwards to local topic /udp/report

import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import socket
import fcntl
import struct
import psutil
import threading
import requests
import sys
from urllib import urlopen

from time import sleep

MON_PATH = '/udp/report'

MQTT_SERVER = "pi4001"

uclient = 1;

prog = ""

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

def extendmjs(mjs):
    myhostname = socket.gethostname()
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('8.8.8.8', 1))  # connect() for UDP doesn't send packets
    ipaddr = s.getsockname()[0]
    #ipaddr = socket.gethostbyname_ex(hostname)[2]
    url = 'http://myip.dnsomatic.com'
    info = requests.request('GET',url)
    extaddr = info.text
    # create the response
    
    mjs["hostname"] = myhostname
    mjs["ipaddr"] = ipaddr
    mjs["extaddr"] = extaddr
    #rsp["function"] ="display driver"
    #rsp["inputs"] = DISPLAY_PATH
    #rsp["outputs"] = BUTS_PATH
    #rsp["control"] = CONTROL_PATH
    return mjs
    
#run this in main loop now
def setupUdp (uport):
    global uclient

    #uport = 37020
    uclient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    uclient.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    uclient.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    uclient.bind(("",uport))

def udpThread(v, stop):
    global uclient
    global stop_thread
    while stop_thread == False:
        data,addr = uclient.recvfrom(1024)
        print ( "%s received udp message :[%s] from [%s]" % (prog, data,addr))
        mjs, res = msgjson(data)
        print ( "decode udp message :[%s]" % res)
        if res == "OK":
            print res

            try:
                mjs = extendmjs(mjs)
                msg = json.dumps(mjs);
                print " Sent msg to " + MON_PATH
                print msg
            
                publish.single(MON_PATH,msg,hostname=MQTT_SERVER)
            except:
                print "decode failed"
    print "thread finished"

        




if __name__ == "__main__":
    global stop_thread
    stop_thread = False
    runMe = True
    prog = sys.argv[0]
    print prog
    setupUdp (37030)
    t = threading.Thread(target=udpThread, args=(1,lambda: stop_thread))
    t.start()
    
    try:
        while runMe:
            sleep(0.5)    

                
    except KeyboardInterrupt:
        #global stop_thread
        print 'quitting'
        stop_thread = True
        runMe = False
        #socket.close(uclient)
        t.join()   
   

