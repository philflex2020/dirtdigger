# Simple demo creating an array of sonar data

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

Test this with 
mosquitto_pub -h localhost -t MOTOR_chan -m '{"cmd":"run","speed":0.5,"heading":370, "steer":-0.3}'

get status with mosquitto_sub -h localhost -t /sensor/data -m 
'{"replyto":"phil","reply-topic":"MOTOR_stat"}'

mosquitto_sub -h localhost -v -t "/sensor/data"

m = {};
m["labels"] = ['2006', '2007', '2008', '2009', '2010', '2011', '2012'];
m["data"] = [[28, 48, 40, 19, 86, 27, 90]];
m["series"] = ['Series A'];
return {payload:[m],topic:msg.topic};

# Import the PCA9685 module.

"""
from __future__ import division
import time
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import threading
import json
import Adafruit_PCA9685

# Uncomment to enable debug output.
#import logging
#logging.basicConfig(level=logging.DEBUG)

# Initialise the PCA9685 using the default address (0x40).

pwm = Adafruit_PCA9685.PCA9685()
pwm.set_pwm_freq(60)

# Alternatively specify a different address and/or bus:
#pwm = Adafruit_PCA9685.PCA9685(address=0x41, busnum=2)

# Configure min and max servo pulse lengths
#servo_min = 150  # Min pulse length out of 4096
#servo_max = 600  # Max pulse length out of 4096

g_vals = [28, 48, 40, 19, 86, 27, 90]

print len(g_vals)


MQTT_SERVER = "localhost"
SENSOR_DRIVE = "/sensor/drive"
SENSOR_DATA = "/sensor/data"

g_send = 0;

def sensor_send( msg ):
	jmsg = json.dumps(msg)
	publish.single(SENSOR_DATA,jmsg,hostname=MQTT_SERVER)

def sensorThread(val, stop):
        global g_send
	global g_vals
	g_sval =  100
        snum = 2
	# todo scan sonar sensor
	while True:
		g_sval = g_sval+ 15
		if g_sval > 500:
			g_sval = 100
        	pwm.set_pwm(snum, 0, g_sval)
		print g_sval


                i = 0;
		for v in g_vals:
			g_vals[i] = g_vals[i] + 10
			if g_vals[i] > 300:
				g_vals[i] = 10
			i = i+1

		m = {}
        	m["labels"] = ['-45', '-30', '-15', '0', '15', '30', '45'];
        	m["data"] = [g_vals];
        	m["series"] = ['Sensor Scan'];
        	#if g_send :
                if 1:
            		sensor_send([m])
            		g_send = 0;
		if stop():
			print "sensorThread end"
			break
		time.sleep(1)


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(MQTT_PATH1)
    client.subscribe(SENSOR_DRIVE)
    print("Subscribed to " + SENSOR_DRIVE)
#    client.subscribe(SENSOR_DATA)
#    print("Subscribed to " + SENSOR_DATA)

# '{"data":[{<servo_num>,<pulse>},]}'
# The callback for when a PUBLISH message is received from the server.

def on_message(client, userdata, msg):
    global g_vals
    global g_send
    #global gsteer
    #global gheading
    #global gsendit

    if msg.topic == SENSOR_DRIVE:
        print(msg.topic+" SENSOR->"+str(msg.payload) + "<<")
        jmes = json.loads(msg.payload)

        print jmes
        g_send = 1;
        #if "data" in jmes.keys():
        #    vals =jmes["data"]
        #    print vals
        #    for val in vals.keys():
        #        print "servo : " + val + " pulse :" + str(vals[val])
        #        #print " servo "+str(val[0]) + "pulse " + str(val[1])
        #        g_vals[val] = vals[val]
        #        snum=int(val)
        #        pwm.set_pwm(snum, 0, vals[val])
        #        #set_servo_pulse(snum, vals[val])

if __name__ == "__main__":

    t = threading.Thread(target=sensorThread, args=(1,lambda: stop_thread))
    t.start()

    #time.sleep(5.0)
    stop_thread = False
    #t.join()
    #servo_test()
    
    print(" starting mqtt stuff\n")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, 1883, 60)
    #print client
    try:
    	client.loop_forever() 
    except KeyboardInterrupt:
       print " quitting"
       stop_thread = True

