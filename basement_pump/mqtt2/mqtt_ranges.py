"""
https://tutorials-raspberrypi.com/raspberry-pi-mqtt-broker-client-wireless-communication/
There are several applications that can be used to send and receive through MQTT, but the simplest on the Raspberry Pi is probably Mosquitto. 
We will install this on the Raspberry Pi first:

sudo apt-get install -y mosquitto mosquitto-clients

mosquitto_sub -h localhost -v -t test_channel

mosquitto_pub -h localhost -t test_channel -m "Hello Raspberry Pi"
mosquitto_sub -h localhost -v -t "\$SYS/#"

sudo pip install paho-mqtt
or
git clone https://github.com/eclipse/paho.mqtt.python.git

cd paho.mqtt.python

python setup.py install

This version will accept angle , range information from a sensor
It will build up a list of objects from that information 
This version will simply display messages (delivered in json) for the lcd

Teset this with 
mosquitto_pub -h localhost -t Range_channel -m '{"key":"sensor","sensor":"sens1","angle":45,"range": 32}'

"""
#import RPi_I2C_driver

import paho.mqtt.client as mqtt
import json
import math

MQTT_SERVER = "localhost"
#MQTT_PATH1 = "test_channel"
MQTT_PATH2 = "Range_channel"

#mylcd = RPi_I2C_driver.lcd()

#json.dumps
SW_PATH = "SWITCH_in"
switch = {}
sensors = {}
# use round to limit defs
# when we detct a point we also add it to pArray

#for each sensor we'll have a list of objects
# each object is constructed of a range and angle from which we get x,y offset
#
# we will have an dict of x items
# given an x, y point  e will add a parray at the rounded y value
# then add the point at the rounded x location

class Pitem:
    def __init__(self, xval, yval):
        self.x = xval
        self.y = yval
        self.count = 1
    
    
class Point:
    def __init__(self, angle, range):
        self.range =  range
        self.angle =  angle
        self.x = range * math.sin(math.radians(angle))
        self.y = range * math.cos(math.radians(angle))
        self.dx = 0
        self.dy = 0
        print(" new object X:" + str(self.x) + " y:" + str(self.y) + "  created")

    def update(self,angle, range):
        self.range =  range
        self.angle =  angle
        oldx = self.x
        oldy = self.y
        self.x = range * math.sin(math.radians(angle))
        self.y = range * math.cos(math.radians(angle))
        self.dx = oldx - self.x
        self.dy = oldy - self.y
        print(" updated object X:" + str(self.x) + " y:" + str(self.y) + "  created")
 
 
        
class Sensor:
    def __init__(self, name):
        self.name =  name
        self.points = {}

    def addPoint(self, angle, range):
        if angle in self.points.keys():
            point = self.points[angle]
            point.update(angle,range)
            print(" point updated ")
        else:
            self.points[angle] = Point(angle,range)
            print(" new point added size :" + str(len(self.objects)))
        print self.points
        
    def getSize(self):
        return len(self.points)
        
yPoints = {}

def addPoint(xval, yval):
    global yPoints
    if round(yval,2) not in yPoints.keys():
        yPoints[round(yval,2)] = {}
    xPoints = yPoints[round(yval,2)]
    if round(xval,2) not in xPoints.keys():
        xPoints[round(xval,2)] = Pitem(xval,yval)
    pi = xPoints[round(xval,2)]
    pi.count+=1
             


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    print("subscribed to mqtt_path2: ["+ MQTT_PATH2+"]")
    print("subscribed to switch_path: ["+ SW_PATH+"]")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(MQTT_PATH1)
    client.subscribe(MQTT_PATH2)
    client.subscribe(SW_PATH)

 # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global switch
    print(msg.topic+" Range->"+str(msg.payload))
    
    jmes = json.loads(msg.payload)
    if jmes['key'] == 'switch':
        switch[jmes['chan']]=jmes['value']
    if jmes['key'] == 'sensor':
        print(" sensor->"+ jmes["sensor"])
        print(" angle->"+ str(jmes["angle"]))
        print(" range->"+ str(jmes["range"]))
        if jmes["sensor"] not in sensors.keys():
            print (" Adding new sensor" )
            sensors[jmes["sensor"]] = Sensor(jmes["sensor"])
        else:
            print (" sensor already present")

        foo = sensors[jmes["sensor"]]
        print foo
        foo.addPoint(jmes["angle"], jmes["range"])
        print (" size " + str(foo.getSize()))
        
            
        #mylcd.backlight(0)
        #mylcd.lcd_display_string(jmes["line1"],1)
        #mylcd.lcd_display_string(jmes["line2"],2)
    # more callbacks, etc

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_SERVER, 1883, 60)
#client.loop_start
 # Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.

# Other loop*() functions are available that give a threaded interface and a
# manual interface.
print("subscriing to mqtt_path2: ["+ MQTT_PATH2 + "]")
print("subscriing to switch_path: ["+ SW_PATH + "]")

client.loop_forever()

###

#import paho.mqtt.publish as publish

#MQTT_SERVER = "127.0.0.1"
#MQTT_PATH = "test_channel"

#publish.single(MQTT_PATH, "Hello World!", hostname=MQTT_SERVER)
###
