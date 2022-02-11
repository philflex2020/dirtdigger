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

mqtt_data

this is a registery of data items.

each data item has an id which can be used like a pointer

mqData = {}

def addMqData(item):
   global mqData
   mqData[len(mqData)] = item

each type will have a ref too

a data item will belong to a named system and have a name, type and a value
we'll also have a list of actions on a value change 

set myrover:corner_steering:front_left:angle 25

this will create a group 
   myrover
which will have a group
  corner_steering

which will have a group
  frount_left

which will have an variable called angle


"""
#import paho.mqtt.client as paho

import json
import time
import threading
import mqtt_data as mqd


MQTT_PATH3 = "data_channel"
MQTT_PATH4 = "data_reply"



def mqttThread(val, stop):
    global client
    oldval={}
    ticks = 8
    mcount = 0
    while True:
        ticks += 1
        print "ticks " + str(ticks)
        if stop():
            print " mqtt thread exit"
            break
        if ticks==5:
            print "get"
            dvars = {}
            dvars["pi2:gpio24"]=str(mcount+10)
            mqd.getVars(MQTT_PATH3, MQTT_PATH4, dvars)
        if ticks > 10:
            print "pub"
            #vars = []
            #vars.append(mqd.getvar("pi2:gpio23",str(mcount)) )
            #msg = mqd.setMsg(vars)
            #print msg
            #jq = json.dumps(msg)
            #mqd.publish(MQTT_PATH3, jq)
            dvars = {}
            dvars["pi2:gpio24"]=str(mcount+10)
            mqd.setVars(MQTT_PATH3, dvars)
            ticks = 0
            mcount += 1
            
        time.sleep(1.0)

if __name__ == "__main__":
    stop_thread = False
    mqd.addSub(MQTT_PATH3)
    mqd.addSub(MQTT_PATH4)

    t = threading.Thread(target=mqttThread, args=(1,lambda: stop_thread))
    t.start()
    try:

        print "test vars"
        jq = mqd.testSetVars()
        jq2 = mqd.testSetVars2()
        print "run jq"

        print jq
        mqd.runSet(jq)
        print "show vars once "
        msg = mqd.showVars()
        print msg
        
        print "run jq2"
        print jq2
        mqd.runSet(jq2)
        
        print "show vars second time"
        msg = mqd.showVars()
        print msg
        print "test client"
        mqd.runConnect(mqd.MQTT_SERVER, 1883, 60)  

        print "after test client"
        while True:
            time.sleep(1.0)
        
    except KeyboardInterrupt:
        print 'quitting'
        mqd.disconnect()
        mqd.loop_stop()
        stop_thread = True
        t.join()

    

    ###

    #import paho.mqtt.publish as publish

    #MQTT_SERVER = "127.0.0.1"
    #MQTT_PATH = "test_channel"

    #publish.single(MQTT_PATH, "Hello World!", hostname=MQTT_SERVER)
    ###
