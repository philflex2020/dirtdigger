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

inputPaths = {}

def setup(host, task):
    global hostname
    global taskname
    
    hostname = host;
    taskname = task
    
def reportFcn(msg, resp):
    print " running reportFcn...",
    print msg.topic
    print msg

def setupFcn(msg,resp):
    print " running setupFcn...",
    print msg
    
def controlFcn(msg,resp):
    print " running controlFcn...",
    print msg

def inputFcn(msg, resp):
    print " running inputFcn...",
    print msg

def outputFcn(msg, resp):
    print " running outputFcn...",
    print msg
    
    
#inputPaths["/udp/report"]  = ("/udp/report",reportFcn)
#addPath("setup", setupFcn)
#addPath("control", controlFcn)
#addPath("output", outputFcn)
#addPath("input", inputFcn)


resp ={}

def reportMsg(path,msg):
    print "trigger report"
    resp[path.topic]="some resp"

def dummyMsg(path, msg):
    print "trigger dummy"
    resp[path.topic]="some dummy"

def setupResp(path, msg):
    print "trigger setup"
    resp[path.topic]="some setup"

def addPath(path, fcn, resp=dummyMsg):
    global inputPaths
    global hostname
    global taskname
    
    thisPath = "/"+hostname+"/"+taskname+"/"+path
    inputPaths[thisPath]  = (thisPath, fcn, resp)



#    publish.single(path, jmsg, hostname=MQTT_SERVER)

    
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(MQTT_PATH1)
    for path in inputPaths.keys():
        client.subscribe(path)
        print "subscribed to "+ path

 # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("topic->"+ msg.topic+" payload->"+str(msg.payload))
    try:
        jmes = json.loads(msg.payload)
    except:
        print "not json"
        jmes = {}
        
    if msg.topic in inputPaths.keys():
        fcn = inputPaths[msg.topic][1]
        resp = inputPaths[msg.topic][2]
        print "running function",
        print fcn
        fcn (msg, resp)
        


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

def connect(server, port, timeout):
    global client
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(server, port, timeout)

def loop_forever():
    global client
    client.loop_forever()

        
if __name__ == "__main__":
    global stop_thread
    global resp
    stop_thread = False
    setup("pibot01", "gpios")
    inputPaths["/udp/report"]  = ("/udp/report",reportFcn, reportMsg)
    addPath("setup",   setupFcn, setupResp)
    addPath("control", controlFcn)
    addPath("output",  outputFcn)
    addPath("input",   inputFcn)
    t = threading.Thread(target=msgThread, args=(1,lambda: stop_thread))
    t.start()


    try:
        client = mqtt.Client()
        client.on_connect = on_connect
        client.on_message = on_message
        client.connect(MQTT_SERVER, 1883, 60)
        
        # Blocking call that processes network traffic, dispatches callbacks and
        # handles reconnecting.
        
        # Other loop*() functions are available that give a threaded interface and a
        # manual interface.
        client.loop_forever()
        
    except KeyboardInterrupt:
        print 'quitting'
        stop_thread = True
        t.join()   
        
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
    
