# this is an attempt to map a /x/y/z uri to a jquery object
#
# or rather give uri access to a json object
#

# a configuration message will set up
# gpio numbers direction names etc
# we need a config copy and change detect
# sample config message
#  gpios
#import paho.mqtt.client as mqtt
#import paho.mqtt.publish as publish


import mqtt2_task_fcn as mqtt2
import json
#import socket
import fcntl
import struct
import psutil
#import RPi.GPIO as GPIO
import time
import argparse
import threading 
import sys

# originally gpios
# copied to motor
# motor functions are speed ( +/- or 0)
# motor functions are pos ( +/- or 0)
#

MQTT_SERVER = "127.0.0.1"
# /udp/report
# /udp/<hostname>/gpio/setup


#hostname="xxx"
#taskname = "yyy"

gpios = {}
gpiomsg = {}
swi1 = {}
swi2 = {}
led1 = {}
led2 = {}

gpios["FP_Switch_1"] = swi1
swi1["Pin"]=23
swi1["Dir"]="Input"
swi1["PD"]="PullUp"

gpios["FP_Switch_2"] = swi2
swi2["Pin"]=27
swi2["Dir"]="Input"
swi2["PD"]="PullDown"

gpios["FP_Led_1"] = led1
gpios["FP_Led_2"] = led2
led2["Pin"]=4
led2["Dir"]="Output"
led2["Type"]="OC"
led2["Value"]="1"

led1["Pin"]=6
led1["Dir"]="Output"
led1["Type"]="OC"
led1["Value"]="1"

jgpios = json.dumps(gpios)

print jgpios
gpios["FP_Switch_2"]["Value"] = 1



jgpios = json.dumps(gpios)
print jgpios




uris = {}
urids = {}
gpiomsg = {}
gpiomsg["FP_Switch_1"] = {}
gpiomsg2 = {}
gpiomsg2["FP_Switch_1"] = {}
gpiomsg["FP_Switch_1"]["Value"] = 1234
gpiomsg2["FP_Switch_1"]["Pin"] = 12

# register a URI key can be none or a real key
# the base object is registered with the uri referencing a keyed object inside the base.

def regUri(uri, obj, key):
    if uri not in uris.keys():
        id = len(uris)
        id += 1
        uris[uri] =[id, obj, key]
        urids[id] = uri
    return uris[uri][0]

def getUri(uri):
    if uri not in uris.keys():
        print "get: no uri found"
        return None
    if uris[uri][2] is None:
        #return the whole object
        return uris[uri][1]
    try:
        rval =  uris[uri][1][uris[uri][2]]
    except:
        print "get: no uri found"
        rval = ""
    return rval

def setUri(uri, value):
    if uri not in uris.keys():
        print "set: no uri found"
        return
    if uris[uri][2] is None:
        #set the whole object
        # todo check for a dict
        uris[uri][1] = value
        return
    try:
        uris[uri][1][uris[uri][2]] = value
    except:
        print "Set: no uri [%s] found " % uri
        
import collections

#same as merge but publish uris
def mergeMsgUri(old, new, uri):
    for k,v in new.iteritems():
        if(k in old and isinstance(old[k], dict)
           and isinstance(old[k], collections.Mapping)):
            newuri = uri + "/" + k
            id = regUri(newuri, old[k], None)
            print "\n\n Dict uri [" + newuri + "] id:" + str(id) 
            mergeMsgUri(old[k], new[k], newuri)
        else:
            old[k] = new[k]
            newuri = uri + "/" + k
            id = regUri(newuri, old, k)
            print "\n\nuri [" + newuri + "] id:" + str(id) 
            print " value = " ,
            print uris[newuri][1][uris[newuri][2]]
            print "old[k] 3456 "


def showUris():
    for id in urids.keys():
      print "ID [%d] URI [%s]"% (id, urids[id])

def mergeMsg(old, new):
    for k,v in new.iteritems():
        if(k in old and isinstance(old[k], dict)
           and isinstance(old[k], collections.Mapping)):
            mergeMsg(old[k], new[k])
        else:
            old[k] = new[k]
            

def isDict ( e) :
    try:
        x = e.items()
        return True
    except(TypeError):
        return False
    except(AttributeError):
        return False
    
def copyMsg(root, new):
    for k in new.keys():
        if isDict(k):
            if k not in root.keys():
                root[k] = {}
            copyMsg(root[k], new[k])
        else:
            root[k] = new[k]

            
            
print swi2
mergeMsgUri(gpios, gpiomsg, "/gpios")
mergeMsgUri(gpios, gpiomsg2, "/gpios")

setUri("/gpios/FP_Switch_1/Pin", 14)
foo = getUri("/gpios/FP_Switch_1")


jgpios = json.dumps(gpios)

print jgpios

print "foo ==="
print foo


# this creates all the URIs for the system
mergeMsgUri(gpios, gpios, "/gpios")


myuri = urids[18]
foo = getUri(myuri)

print
print myuri
print "foo from myuri ==="
print foo

showUris()

sys.exit(0)    



inputPaths = {}

def xxsetup(host, task):
    global hostname
    global taskname
    
    hostname = host;
    taskname = task
    
def reportFcn(msg, resp):
    print " running gpios reportFcn...",
    print msg.topic
    print msg

def setupFcn(msg,resp):
    print " running gpios setupFcn...",
    print msg
    
def controlFcn(msg,resp):
    print " running gpios controlFcn...",
    print msg

def inputFcn(msg, resp):
    print " running gpios inputFcn...",
    print msg

def outputFcn(msg, resp):
    print " running gpios outputFcn...",
    print msg
    
    
#inputPaths["/udp/report"]  = ("/udp/report",reportFcn)
#addPath("setup", setupFcn)
#addPath("control", controlFcn)
#addPath("output", outputFcn)
#addPath("input", inputFcn)




def reportMsg(path,msg):
    global resp
    print "trigger report"
    resp[path.topic]="some resp"

def dummyMsg(path, msg):
    print "trigger dummy"
    resp[path.topic]="some dummy"

def setupResp(path, msg):
    print "trigger setup"
    resp[path.topic]="some setup"


#    publish.single(path, jmsg, hostname=MQTT_SERVER)

def msgThread(val, stop):
    oldval={}
    ticks = 0
    while True:
        ticks += 1
        if stop():
            print " thread exit"
            break
        

        if sendit:
            print "sent"
            gpio_send(SW_PATH, chan, value)
            
        time.sleep(1.0)
        #publish.single(path, jmsg, hostname=MQTT_SERVER)
    

if __name__ == "__main__":
    global resp
    resp ={}
    mqtt2.setup("pibot01", "gpios")
    mqtt2.inputPaths["/udp/report"]  = ("/udp/report",reportFcn, reportMsg)
    mqtt2.addPath("setup",   setupFcn, setupResp)
    mqtt2.addPath("control", controlFcn)
    mqtt2.addPath("output",  outputFcn)
    mqtt2.addPath("input",   inputFcn)


    mqtt2.connect(MQTT_SERVER, 1883, 60)
    mqtt2.loop_forever()
        
        
#    while True:
#        pwmd +=padd
#        if pwmd >= 80:
#            padd = -20
#        if pwmd < 20:
#            padd = 20
                
#        #pwm.ChangeDutyCycle(pwmd)
#        #GPIO.output(chan18, GPIO.HIGH)
#        GPIO.output(chan23, GPIO.LOW)
#        print GPIO.input(chan17)
#        time.sleep(1)
#        GPIO.output(chan23, GPIO.HIGH)
#        #GPIO.output(chan18, GPIO.LOW)
#        time.sleep(1)

#t.stop()
    
