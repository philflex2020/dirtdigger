"""
https://tutorials-raspberrypi.com/raspberry-pi-mqtt-broker-client-wireless-communication/
There are several applications that can be used to send and receive through MQTT, but the simplest on the Raspberry Pi is probably Mosquitto. 
We will install this on the Raspberry Pi first:

sudo apt-get install -y mosquitto mosquitto-clients
mosquitto_sub -h localhost -v -t test_channel

mosquitto_pub -h pine64-002 -v -t "/pine64-002/foo" -m "hi there"

to install paho-mqtt

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

set /myrover/corner_steering/front_left/angle 25

this will create a group 
   myrover
which will have a group
  corner_steering

which will have a group
  frount_left

which will have an variable called angle

we subscribe to a path on a selected host

this option will NOT collect data items into a single message
the path will be the data item name

All this is a little complex we'll have to refine it.


/pi2/gpios {"set":{"gpio23":"on","gpio24":"off"}
/pi2/gpios {"get":{"gpio23","gpio24"},"reply":"/pi0/pi2/gpios"}
/pi2/gpios/gpio23 {"set":"on"}

the host to send these to is pi2
the the get repies to /pi0/gpios


"""
import paho.mqtt.client as paho

import json
import time
import threading 
import sys 


MQTT_SERVER = "localhost"
MQTT_PATH1 = "/pi2/gpios/#"
MQTT_PATH1R = "test_reply"
MQTT_PATH2 = "LCD_channel"
MQTT_PATH3 = "data_channel"
MQTT_PATH4 = "/pi2/pine_channel"

mqData = {}
mqType = {}
mqSubs =[]
mqHosts = {}

class MqHost:
    def __init__(self, name):
        self.port=1883
        self.ptime=60
        self.paths={}
        self.name=name
        self.connected = 0
        self.client = 0
        
class MqPath:
    def __init__(self, name, host):
        self.host = host
        self.name=name
        self.connected = 0


def addSub(name, host=MQTT_SERVER, port=1883, ptime=60):
    global mqHosts
    if host not in mqHosts.keys():
        mqh = MqHost(host)
        mqHosts[host] = mqh
    else:
        mqh = mqHosts[host]
        
    if name not in mqh.paths.keys():
        mqp = MqPath(name, host)
        mqh.paths[name] = mqh
    else:
        mqp = mqh.paths[name]

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


def setMsg(vars, path=MQTT_PATH1, host=MQTT_SERVER):
    msg = {}
    msg["cmd"] = "set"
    msg["path"] = path
    msg["host"] = host

    msg["grp"] = "myGroup"
    msg["vars"] = vars
    return msg

def getMsg(vars, path=MQTT_PATH1, rpath=MQTT_PATH1R, host=MQTT_SERVER):
    msg = {}
    msg["cmd"] = "get"
    msg["path"] = path
    msg["grp"] = "myGroup"
    msg["reply"] = rpath
    msg["host"] = host
    msg["vars"] = vars
    return msg

def mkvar(name,value):
    v = {}
    #v["name"]= name
    v["value"]=value
    return v 

def mksvar(action,name,value):
    v = {}
    p={}
    #v["name"]= name
    p["value"]=value
    p["action"]=action
    v["path"]=name
    v['payload']=json.dumps(p)
    return v 

def testSetVars():
    vars = []
    vars.append(mkvar("/pi2/gpios/gpio21","on") )
    vars.append(mkvar("/pi2/gpios/gpio22","on") )
    vars.append(mkvar("/pi2/gpio/gpio23","off") )
    msg = setMsg(vars)
    
    return msg

def testSetVars2():
    vars = []
    vars.append(mkvar("/pi2/gpios/gpio21","on again") )
    vars.append(mkvar("/pi3/gpios/gpio22","on") )
    vars.append(mkvar("/pi3/gpios/gpio23","off") )
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
        

    msgs = setMsg(jq["vars"])
    print "setMsg OK..",
    #print msg
    jqs = json.dumps(msg)
    print "dumps OK..",
    #publish(jqs, jq["reply"])
    print "publish OK "

    #print msg    
    

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print "Connected with result code "+str(rc)
    connectPaths(client, 1)


    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(MQTT_PATH1)
    #client.subscribe(MQTT_PATH2)
    #client.subscribe(MQTT_PATH3)

 # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print "Got Data Topic-->"+msg.topic+"<...->"+str(msg.payload)
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
        vars.append(mkvar(var,dvars[var]))
    msg = setMsg(vars)
    #print msg
    jq = json.dumps(msg)
    publish(jq, path)

def getVars(path, rpath, dvars):
    vars = []
    for var in dvars.keys():
        vars.append(mkvar(var,dvars[var]))
    msg = getMsg(vars, rpath)
    #print msg
    jq = json.dumps(msg)
    publish(jq, path)

def sendVars(vars, host=MQTT_SERVER):
    for var in vars:
        path = var["path"]
        print "publish " + path
        #see mksvar
        payload = var["payload"]
        publish(payload, path, host)

            
def mqttThread(val, stop):
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
            vars.append(mksvar("set", "/pi2/gpios/gpio23",str(mcount)) )
            vars.append(mksvar("set", "/pi2/gpios/gpio21",str(mcount+3)) )
            sendVars(vars)
            #client.publish(MQTT_PATH3, jq)
            ticks = 0
            mcount += 1
            
        time.sleep(1.0)

def disconnectHosts():
    global mqHosts
    for host in mqHosts.keys():
        mqh = mqHosts[host]
        if mqh.connected == 1:
            mqh.client.disconnect()
            mqh.client.loop_stop()
            mqh.connected = 0

def connectHost( host):
    global mqHosts
    if host not in mqHosts.keys():
        mqh = mqHosts[host]
        if mqh.connected == 0:
            try:
                mqh.client = runConnect(mqh.name, mqh.port, mqh.ptime)
                mqh.connected = 1
                print "Connected to host [" + host + "]"
                #connectHpaths(mqh)
                path = MQTT_PATH1
                mqh.client.subscribe(path)
                print "Subscribed to path [" + path + "]"


            except:
                print "Unable to connect to host [" + host +"]"

                
def addHostSun(host, path):
    global mqHosts
    if host not in mqHosts.keys():
        mqh = mqHosts[host]
        if mqh.connected == 1:
            mqh.client.subscribe(path)
    
def connectHosts():
    global mqHosts
    for host in mqHosts.keys():
        mqh = mqHosts[host]
        if mqh.connected == 0:
            try:
                mqh.client = runConnect(mqh.name, mqh.port, mqh.ptime)
                mqh.connected = 1
                print "Connected to host [" + host + "]"
                #connectHpaths(mqh)
                path = MQTT_PATH1
                mqh.client.subscribe(path)
                print "Subscribed to path [" + path + "]"


            except:
                print "Unable to connect to host [" + host +"]"
            
            
def connectPaths(client, force):
    global mqHosts
    for host in mqHosts.keys():
        mqh = mqHosts[host]
        if force or mqh.connected == 1:
            if mqh.client == client:
                connectHpaths(mqh)

def connectHpaths(mqh):
    paths = mqh.paths
    for paths in paths.keys():
        if path.connected == 0:
            try:
                mqh.client.subscribe(path.name)
                path.connected = 1
                print "host " + mqh.name + "subscribed to path [" + path.name + "]"

            except:
                print "host " + mqh.name + "NOT subscribed to path " + path.name


def connectHpath(mqh, path):
    paths = mqh.paths

    if path in paths.keys():
        pth = paths[path]
        if pth.connected == 0:
            try:
                mqh.client.subscribe(pth.name)
                pth.connected = 1
                print "host " + mqh.name + "subscribed to path [" + pth.name + "]"
                
            except:
                print "host " + mqh.name + "NOT subscribed to path " + pth.name

client_id = 21

def runConnect(server, port=1883, ptime=60):
    global client_id
    cid = "client-"+ str(client_id)
    client_id += 1
    print "connecting [" + cid + "] to server [" + server +"]"
    
    client = paho.Client(cid)
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

def findHostClient(host):
    global mqHosts
    if host in mqHosts.keys():
        mqh = mqHosts[host]
        if mqh.connected == 1:
            return mqh.client
    return 0
    
def publish(jq, path, host=MQTT_SERVER):
    client = findHostClient(host)
    if client:
        client.publish(path, jq)
        print "published.. to "+ host
    else:
        print "could not find host "+ host

def getHost(host):
    mqh = None
    if host not in mqHosts.keys():
        mqh = MqHost(host)
        mqHosts[host] = mqh
    else:
        mqh = mqHosts[host]

    if mqh.connected == 0:
        try:
            mqh.client = runConnect(mqh.name, mqh.port, mqh.ptime)
            mqh.connected = 1
            print "Connected to host [" + host + "]"
        except:
            print "Unable to onnected to host [" + host + "]"
        
    return mqh

def newSetVars(path, vars):
    psplit = path.split("/")
    host = psplit[1]
    print psplit,
    print "host ["+ host + "]"
    svars = {}
    svars["set"] = vars
    msg = json.dumps(svars)
    print msg
    mqh = getHost(host)

    if mqh and mqh.connected == 1:
        publish(msg, path, host)
        print "publish done path " + path
    
def newGetVars(path, vars, reply):
    psplit = path.split("/")
    host = psplit[1]
    print psplit,
    print "host ["+ host + "]"
    vlist = []
    for v in vars.keys():
        vlist.append(v)
        
    svars = {}
    svars["get"] = vlist
    svars["reply"] = reply
    msg = json.dumps(svars)
    print msg
    mqh = getHost(host)

    if mqh and mqh.connected == 1:
        addSub(path, host)
        connectHpath(mqh, path)
        mqh.client.loop_start()
        publish(msg, path+"/d", host)
        print "publish done path [" + path + "]"

def newSetVar(path, val):
    psplit = path.split("/")
    host = psplit[1]
    print psplit,
    print "host ["+ host + "]"
    msg = val
    print msg
    mqh = getHost(host)

    if mqh and mqh.connected == 1:
        publish(msg, path, host)
        print "publish done path " + path
    
            
if __name__ == "__main__":
    stop_thread = False
    addSub(MQTT_PATH1)
    addSub(MQTT_PATH2)
    addSub(MQTT_PATH3)
    try:
        if sys.argv[1] == "node":
            print "connecting to "+ sys.argv[2]
            # in this mode just open and listen
            mqh = getHost(sys.argv[2])
            print "connected to "+ mqh.name
            addSub('/pine64-002/#', mqh.name)
            print "added sub "+ mqh.name

            time.sleep(10)

            print "quitting"
            disconnectHosts()
            print "after disconnect hosts"

            try:
                sys.exit(0)
            except:
                print
                print
                print "exit failed"
                print
                print
            while True:
                time.sleep(1)
                

    except:
         print "oops error"

    # to test run this command on pine64-002
    # mosquitto_sub -h localhost -v -t "/pine64-002/#"
    # /pi2/gpios {"set":{"gpio23":"on","gpio24":"off"}
    #$ mosquitto_sub -h localhost -v -t "/pine64-002/#"
    #
    #/pine64-002/gpios
    #       {"set": {"gpio21": "off", "gpio20": "pulse:50:25", "gpio23": "on"}}
    #/pine64-002/gpios/d
    #       {"reply": "/pine64-001/pi2/gpios",
    #             "get": ["gpio21", "gpio20", "gpio23"]}
    #
    #/pine64-002/gpios/gpio23 on
    # import sys
    vars = {}
    vars["gpio23"] = "on"
    vars["gpio21"] = "off"
    vars["gpio20"] = "pulse:50:25"
    newSetVars("/pine64-002/gpios", vars);

    
    #/pi2/gpios {"get":{"gpio23","gpio24"},"reply":"/pi0/pi2/gpios"}
    reply = "/pine64-001/pi2/gpios"
    newGetVars("/pine64-002/gpios", vars, reply);
    print
    print "send reply NOW"
    print
    time.sleep(5.0)
    #/pi2/gpios/gpio23 {"set":"on"}
    newSetVar("/pine64-002/gpios/gpio23", "on");
    time.sleep(5.0)
    print 'quitting'
    disconnectHosts()
    print "after disconnect hosts"
    
    #stop_thread = True
    #t.join()
    sys.exit(0)
    #addSub(MQTT_PATH4, "pine64-002")
    #addSub(MQTT_PATH4, "localhost")

    t = threading.Thread(target=mqttThread, args=(1,lambda: stop_thread))
    t.start()
    try:

        print "test vars"
        jq = testSetVars()
        jq2 = testSetVars2()
        print "run jq"

        print jq
        #runSet(jq)
        print "show vars once "
        msg = showVars()
        print msg
        
        print "run jq2"
        print jq2
        #runSet(jq2)
        
        print "show vars second time"
        msg = showVars()
        print msg
        print "test client"
        connectHosts()
        #client = runConnect(MQTT_SERVER, 1883, 60)  
        print "after connect hosts"

    
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
        disconnectHosts()
        print "after disconnect hosts"

        stop_thread = True
        t.join()

        

    

    ###

    #import paho.mqtt.publish as publish

    #MQTT_SERVER = "127.0.0.1"
    #MQTT_PATH = "test_channel"

    #publish.single(MQTT_PATH, "Hello World!", hostname=MQTT_SERVER)
    ###
