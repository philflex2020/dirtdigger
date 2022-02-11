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
import paho.mqtt.client as paho

import json
import time
import threading 


MQTT_SERVER = "localhost"
MQTT_PATH1 = "test_channel"
MQTT_PATH2 = "LCD_channel"
MQTT_PATH3 = "data_channel"


mqData = {}
mqType = {}
mqSubs =[]


def addSub(name):
    global mqSubs
    if name not in mqSubs:
        mqSubs.append(name)

class Var:
    def __init__(self, name, value):
        self.name =  name
        self.value =  value
        self.id = 0
        self.callbacks = []
        
    def addCallback(self, cb):
        self.callbacks.append(cb)
        
    def setValue(self, value):
        if value != self.value:
            for cb in self.callbacks:
                value = cb(self,cb,value)
            self.value = value

    def getValue(self):
        return self.value
            
def addMqData(item):
   global mqData
   idx = len(mqData)
   mqData[idx] = item
   return idx

def findMqDataName(name):
    global mqData
    for var in mqData.keys():
        mqv = mqData[var]
        if mqv.name == name:
            return var
    return -1



def addMqType(item):
   global mqType
   idx = len(mqType)
   mqType[idx] = item
   return idx

def mqtt_msg(grp, cmd, vars):
    msg = {}
    msg["cmd"] = cmd
    msg["grp"] = grp
    msg["vars"] = vars
    jq = json.dumps(msq)
    return jq


def setMsg(vars):
    msg = {}
    msg["cmd"] = "set"
    msg["grp"] = "myGroup"
    msg["vars"] = vars
    return msg

def getMsg(vars, rpath, host=MQTT_SERVER):
    msg = {}
    msg["cmd"] = "get"
    msg["grp"] = "myGroup"
    msg["vars"] = vars
    msg["reply"] = rpath
    msg["host"] = host
    return msg

def getvar(name,value):
    v = {}
    v["name"]= name
    v["value"]=value
    return v 

def testSetVars():
    vars = []
    vars.append(getvar("pi2:gpio21","on") )
    vars.append(getvar("pi2:gpio22","on") )
    vars.append(getvar("pi2:gpio23","off") )
    msg = setMsg(vars)
    
    return msg

def testSetVars2():
    vars = []
    vars.append(getvar("pi2:gpio21","on again") )
    vars.append(getvar("pi3:gpio22","on") )
    vars.append(getvar("pi3:gpio23","off") )
    msg = setMsg(vars)
    
    return msg


def showVars():
    global mqData
    msg = ""
    for var in mqData.keys():
        msg += str(var) 
        msg += ".."
        mqv = mqData[var]
        msg += mqv.name 
        msg += "==>"
        msg += mqv.value + "\n"
    return msg
    

def runSet(jq):
    msg = ""
    for var in jq["vars"]:
        idx =  findMqDataName(var["name"])
        
        if idx <0:
            
            idx = addMqData(Var(var["name"], var["value"]))
            msg += "added var " + var["name"] + " at idx "+ str(idx) + "\n"
        else:
            msg += "found var " + var["name"] + " at idx "+ str(idx) + "\n" 

        mqv = mqData[idx]
        mqv.setValue(var["value"])
        print msg

def runGet(jq):

    msg = ""
    for var in jq["vars"]:
        idx =  findMqDataName(var["name"])
        
        if idx <0:
            
            idx = addMqData(Var(var["name"], var["value"]))
            msg += "added var " + var["name"] + " at idx "+ str(idx) + "\n"
        else:
            msg += "found var " + var["name"] + " at idx "+ str(idx) + "\n" 
        print "--->" + msg
        mqv = mqData[idx]

        print "mqv =",
        print mqv
        
        var["value"] = mqv.getValue()
        print "var value =",
        print var["value"]
        

    msg = setMsg(jq["vars"])
    print "setMsg OK..",
    #print msg
    jqs = json.dumps(msg)
    print "dumps OK..",
    publish(jqs, jq["reply"])
    print "publish OK "

    #print msg    
    

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    global mqSubs
    for name in mqSubs:
        print("Subscribe to " + name)
        client.subscribe(name)

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(MQTT_PATH1)
    #client.subscribe(MQTT_PATH2)
    #client.subscribe(MQTT_PATH3)

 # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("Got Data Topic-->"+msg.topic+"<...->"+str(msg.payload))

    if msg.topic == 'data_channel':
        try:
            jq=json.loads(msg.payload)
            print "cmd = " + jq["cmd"]
            print "grp = " + jq["grp"]
            print "vars = ",
            print jq["vars"]
            if jq["cmd"] == "sub":
                client.subscribe(jq["grp"])
            elif jq["cmd"] == "set":
                runSet(jq)
            elif jq["cmd"] == "get":
                try:
                    runGet(jq)
                except:
                    print "json runGet failed"
                    
                
        except:
            print "json load failed"

def setVars(path, dvars):
    vars = []
    for var in dvars.keys():
        vars.append(getvar(var,dvars[var]))
    msg = setMsg(vars)
    #print msg
    jq = json.dumps(msg)
    publish(jq, path)

def getVars(path, rpath, dvars):
    vars = []
    for var in dvars.keys():
        vars.append(getvar(var,dvars[var]))
    msg = getMsg(vars, rpath)
    #print msg
    jq = json.dumps(msg)
    publish(jq, path)
            
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
        if ticks > 10:
            print "pub"
            vars = []
            vars.append(getvar("pi2:gpio23",str(mcount)) )
            msg = setMsg(vars)
            print msg
            jq = json.dumps(msg)
            client.publish(MQTT_PATH3, jq)
            ticks = 0
            mcount += 1
            
        time.sleep(1.0)
        
    
def runConnect(server, port=1883, ptime=60):        
    print "connecting"
    global client
    client = paho.Client("client-001")
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(server, port, ptime)
    client.loop_start()
    print "after client.loop_start"

    return client

    # more callbacks, etc
def disconnect():
    global client
    client.disconnect()

def loop_stop():
    global client
    client.loop_stop()

def publish(jq, path, host=""):
    global client
    client.publish(path, jq)

if __name__ == "__main__":
    stop_thread = False
    addSub(MQTT_PATH1)
    addSub(MQTT_PATH2)
    addSub(MQTT_PATH3)

    t = threading.Thread(target=mqttThread, args=(1,lambda: stop_thread))
    t.start()
    try:

        print "test vars"
        jq = testSetVars()
        jq2 = testSetVars2()
        print "run jq"

        print jq
        runSet(jq)
        print "show vars once "
        msg = showVars()
        print msg
        
        print "run jq2"
        print jq2
        runSet(jq2)
        
        print "show vars second time"
        msg = showVars()
        print msg
        print "test client"
        client = runConnect(MQTT_SERVER, 1883, 60)  

        print "after test client"

    
        # Blocking call that processes network traffic, dispatches callbacks and
        # handles reconnecting.

        # Other loop*() functions are available that give a threaded interface and a
        # manual interface.
        #client.loop_forever()
        #client.loop_start()
        while True:
            time.sleep(1.0)
        
    except KeyboardInterrupt:
        print 'quitting'
        client.disconnect()
        client.loop_stop()
        stop_thread = True
        t.join()

    

    ###

    #import paho.mqtt.publish as publish

    #MQTT_SERVER = "127.0.0.1"
    #MQTT_PATH = "test_channel"

    #publish.single(MQTT_PATH, "Hello World!", hostname=MQTT_SERVER)
    ###
