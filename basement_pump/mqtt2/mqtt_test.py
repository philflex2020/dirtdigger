#
# mqtt class test
#

#Libraries
#import paho.mqtt.client as mqtt
#import paho.mqtt.publish as publish
#import RPi.GPIO as GPIO
import json
import time


import mqtt_class as MQ

MQTT_SERVER = "localhost"
MQTT_BASE = "/test"

#g_trigger = 0
#g_seq = 0

#GPIO Mode (BOARD / BCM)
#GPIO.setmode(GPIO.BCM)

#set GPIO Pins
#GPIO_TRIGGER = 22
#GPIO_ECHO = 27

#set GPIO direction (IN / OUT)
#GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
#GPIO.setup(GPIO_ECHO, GPIO.IN)

#def sonar_send(msg):
#	jmsg = json.dumps(msg)
#        publish.single(MQTT_DATA,jmsg,hostname=MQTT_SERVER)

#def on_connect(client,userdata,flags,rc):
#        client.subscribe(MQTT_REQUEST)

#def on_message(client, userdata, msg):
#	global g_seq
#	global g_trigger
#	if msg.topic == MQTT_REQUEST:
#		print ("SONAR> topic: <" + msg.topic +"> payload <"+ msg.payload + ">")
#            	jmsg = json.loads(msg.payload)
#		if "seq" in jmsg.keys():
#			g_seq = jmsg["seq"]
#			g_trigger = 1

#def sonarThread(val,stop):
#	global g_seq
#	global g_trigger
#        print "sonar thread runninig"
#	while True:
#		if g_trigger == 1:

#			print "seq :" + str(g_seq) + " dist : " + str(dist)

def get_data(jmsg):
        test = testdata()
        resp = {}
        resp["test"] = test
        return resp

def get_data2(jmsg):
        test = testdata()
        resp = {}
        resp["test2"] = test
        resp["dummy"] = "dummy"
        resp["time"] = time.time()
        
        return resp


def testdata():

    StartTime = time.time()
    StopTime = time.time()
    TimeElapsed = StopTime - StartTime
    distance = TimeElapsed * 1000

    return distance

#mq = MQ
mq=MQ.mqtt_class("test", get_data)
mq.on("test2", get_data2)

if __name__ == '__main__':
        mq.runThread()

    	try:
         	#while True:
            	print(" testing")
            	dist = testdata()
            	print ("Test Data = %.1f cm" % dist)
		#print "monitor with mosquitto_sub -h xxx -t /sonar/data "
		#print "trigger with mosquitto_pub -h xxx -t /sonar/request -m '{\"seq\": 34}`"
		mq.client_loop()

	        # Reset by pressing CTRL + C
    	except KeyboardInterrupt:
        	print("Test stopped by User")
		mq.stopThread()
		
        	#GPIO.cleanup()

