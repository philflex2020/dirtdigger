
#!/usr/bin/python
'''
**********************************************************************
* Filename    : mqtt_class.py
* Description : mqtt_helper class
* Author      : Phil Wilshire
* Brand       : SDCS
* E-mail      : sysdcs@gmail.com
* Website     : nah 
* Version     : v1.0.0
**********************************************************************
'''

#
# mqtt helper class
# we get scan request then send back data
#

#Libraries
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish

import json
import time
import threading

MQTT_SERVER = "localhost"
MQTT_DATA = "/sonar/data"
MQTT_REQUEST = "/sonar/request"


g_trigger = 0
g_seq = 0

g_trig = 1
mqtt_base = ""
mqtt_req = ""
g_jmsg = 0

stop_thread = False

cond = threading.Condition()
def on_connect(client, userdata, flags, rc):
        global mqtt_req
        client.subscribe(mqtt_req)

def on_message(client, userdata, msg):
	global g_seq
	global g_trig
        global g_jmsg
        global mqtt_req
        
	if msg.topic == mqtt_req:
		print ("SONAR> topic: <" + msg.topic +"> payload <"+ msg.payload + ">")
            	jmsg = json.loads(msg.payload)
		if "seq" in jmsg.keys():
                        global g_seq
			g_jmsg = jmsg
                        cond.acquire()
                        cond.notify()
                        cond.release()
			#g_trig = 1
                        g_seq = jmsg["seq"]
                        print "g_seq 1 =" + str(g_seq)

def mqtt_send(msg):
        global mqtt_data
	jmsg = json.dumps(msg)
        publish.single(mqtt_data,jmsg,hostname=mqtt_host)


def mqttThread(val,stop):
	global g_seq
	global g_trig
        global get_data
        print "mqtt thread runninig"

	while True:
                cond.acquire()
                cond.wait()
                cond.release()
                print "mqtt tick :" + str(g_trig)
		if g_trig == 1:
                        #global g_seq
                        resp = get_data()
			#dist = distance()
			#print "seq :" + str(g_seq) + " dist : " + str(dist)
        		#resp = {}
                        #print "g_seq 2 ="
                        #print str(g_seq)
        		resp["seq"] = g_seq
			#resp["dist"] = dist
			#g_trig = 0
			mqtt_send(resp)
		if stop():
			print " thread exit"
			break
		#time.sleep(1.0)
                
class mqtt_class():
        def __init__(self, mqtt_base, get_dat, host=MQTT_SERVER):
                global client
                global mqtt_req
                global mqtt_data
                global mqtt_host
                global get_data
                get_data = get_dat

                mqtt_req = "/"+mqtt_base+"/req"
                mqtt_data = "/"+mqtt_base+"/data"
                mqtt_host = host
                client = mqtt.Client()
	        client.on_connect =  on_connect
	        client.on_message =  on_message
                client.connect(mqtt_host, 1883, 60)
                print "monitor with mosquitto_sub -h xxx -t " + mqtt_data
		print "trigger with mosquitto_pub -h xxx -t " + mqtt_req +" -m '{\"seq\": 34}`"
        def runThread(self):
                global stop_thread
                self.t = threading.Thread(target=mqttThread, args=(1,lambda: stop_thread))
	        self.t.start()


        def client_loop(self):
                client.loop_forever()

        def stopThread(self):
                global stop_thread
                stop_thread = True
                self.t.join()


