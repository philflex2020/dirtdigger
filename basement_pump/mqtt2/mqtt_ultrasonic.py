#
# mqtt ultrasonic scanner
# we get scan request then send back data
#

#Libraries
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import RPi.GPIO as GPIO
import json
import time
import threading

MQTT_SERVER = "localhost"
MQTT_DATA = "/sonar/data"
MQTT_REQUEST = "/sonar/request"

g_trigger = 0
g_seq = 0

#GPIO Mode (BOARD / BCM)
GPIO.setmode(GPIO.BCM)

#set GPIO Pins
GPIO_TRIGGER = 22
GPIO_ECHO = 27

#set GPIO direction (IN / OUT)
GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)

def sonar_send(msg):
	jmsg = json.dumps(msg)
        publish.single(MQTT_DATA,jmsg,hostname=MQTT_SERVER)

def on_connect(client,userdata,flags,rc):
        client.subscribe(MQTT_REQUEST)

def on_message(client, userdata, msg):
	global g_seq
	global g_trigger
	if msg.topic == MQTT_REQUEST:
		print ("SONAR> topic: <" + msg.topic +"> payload <"+ msg.payload + ">")
            	jmsg = json.loads(msg.payload)
		if "seq" in jmsg.keys():
			g_seq = jmsg["seq"]
			g_trigger = 1

def sonarThread(val,stop):
	global g_seq
	global g_trigger
        print "sonar thread runninig"
	while True:
		if g_trigger == 1:
			dist = distance()
			print "seq :" + str(g_seq) + " dist : " + str(dist)
        		resp = {}
        		resp["seq"] = g_seq
			resp["dist"] = dist
			g_trigger = 0
			sonar_send(resp)
		if stop():
			print " thread exit"
			break
		time.sleep(1.0)

#GPIO.setup(GPIO_ECHO, GPIO.OUT)
#print "trigger on"
#GPIO.output(GPIO_TRIGGER, True)
#time.sleep(1)
#print "echo on"
#GPIO.output(GPIO_ECHO, True)
#time.sleep(1)
#print "echo off"
#GPIO.output(GPIO_ECHO, False)
#time.sleep(1)
#print "triger off"
#GPIO.output(GPIO_TRIGGER, False)

#time.sleep(1)
#print "done"


def distance():
    #return 0
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGER, True)

    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER, False)

    StartTime = time.time()
    StopTime = time.time()

    # save StartTime
    while GPIO.input(GPIO_ECHO) == 0:
        StartTime = time.time()

    # save time of arrival
    while GPIO.input(GPIO_ECHO) == 1:
        StopTime = time.time()

    # time difference between start and arrival
    TimeElapsed = StopTime - StartTime
    # multiply with the sonic speed (34300 cm/s)
    # and divide by 2, because there and back
    distance = (TimeElapsed * 34300) / 2

    return distance

if __name__ == '__main__':
	stop_thread = False
        t = threading.Thread(target=sonarThread, args=(1,lambda: stop_thread))
	t.start()
	client = mqtt.Client()
	client.on_connect =  on_connect
	client.on_message =  on_message
        client.connect(MQTT_SERVER, 1883, 60)

    	try:
         	#while True:
            	print(" testing")
            	dist = distance()
            	print ("Measured Distance = %.1f cm" % dist)
		print "monitor with mosquitto_sub -h xxx -t /sonar/data "
		print "trigger with mosquitto_pub -h xxx -t /sonar/request -m '{\"seq\": 34}`"
		client.loop_forever()

	        # Reset by pressing CTRL + C
    	except KeyboardInterrupt:
        	print("Measurement stopped by User")
		stop_thread = True
		t.join()
        	GPIO.cleanup()
