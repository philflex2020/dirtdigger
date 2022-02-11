
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
#MQTT_DATA = "/sonar/data"
#MQTT_REQUEST = "/sonar/request"


g_trigger = 0
g_seq = 0



mqtt_reqs = {}

stop_thread = False
cond = threading.Condition()

def on_connect(client, userdata, flags, rc):
        global mqtt_reqs
        for m in mqtt_reqs.keys():
                client.subscribe(m)

def on_message(client, userdata, msg):

        global mqtt_reqs
        if msg.topic in mqtt_reqs.keys():
		print ("TEST> topic: <" + msg.topic +"> payload <"+ msg.payload + ">")
            	jmsg = json.loads(msg.payload)
                m = mqtt_reqs[msg.topic]
		m["jmsg"] = jmsg
                m["trig"] = 1
                mqtt_reqs[msg.topic] = m
	
                cond.acquire()
                cond.notify()
                cond.release()

                

def mqtt_send(msg, resp, host):
	jmsg = json.dumps(msg)
        publish.single(resp, jmsg, hostname=host)


def mqttThread(val,stop):
	print "mqtt thread runninig"
        tick = 1
	while True:
                cond.acquire()
                cond.wait()
                cond.release()
                print "mqtt tick :" + str(tick)
                for mk in mqtt_reqs.keys():
                        m = mqtt_reqs[mk]
                        if m["trig"] == 1:
                                resp = m["fcn"](m["jmsg"])
			        m["trig"] = 0
                                mqtt_reqs[mk] = m
			        mqtt_send(resp, m["resp"], m["host"])
		if stop():
			print " thread exit"
			break
                tick += 1
		#time.sleep(1.0)
                
class mqtt_class():
        def __init__(self, mqtt_base, get_dat, host=MQTT_SERVER):
                global client
                global mqtt_reqs
    
                self.mqtt_base = mqtt_base
                self.mqtt_host = host
                
    
                client = mqtt.Client()
	        client.on_connect =  on_connect
	        client.on_message =  on_message
                client.connect(self.mqtt_host, 1883, 60)

                mqtt_req = "/"+mqtt_base+"/req"
                md = {}
                md["fcn"] = get_dat
                md["host"] = self.mqtt_host
                md["jmsg"] = ""
                md["trig"] = 0
                md["resp"] = "/"+mqtt_base+"/data"
                mqtt_reqs[mqtt_req] = md
                self.on("/req", get_dat, "data")
                #print "monitor with mosquitto_sub -h xxx -t " + md["resp"]
		#print "trigger with mosquitto_pub -h xxx -t " + mqtt_req +" -m '{\"seq\": 34}`"
        def runThread(self):
                global stop_thread
                self.t = threading.Thread(target=mqttThread, args=(1,lambda: stop_thread))
	        self.t.start()


        def client_loop(self):
                client.loop_forever()

        def stopThread(self):
                global stop_thread
                stop_thread = True
                cond.acquire()
                cond.notify()
                cond.release()
                self.t.join()

        def on(self, key, func, resp="data", host =""):
                mqtt_req = "/"+self.mqtt_base+"/" + key
                md = {}
                md["fcn"] = func
                md["jmsg"] = ""
                md["trig"] = 0
                if host == "":
                        md["host"] = self.mqtt_host
                else:
                        md["host"] = host
                        
                md["resp"] = "/"+self.mqtt_base+"/" + resp
                mqtt_reqs[mqtt_req] = md
                print "trigger with mosquitto_pub -h "+ md["host"] + " -t " + mqtt_req +" -m '{\"seq\": 34}`"
                print "monitor with mosquitto_sub -h "+ md["host"] + " -t " + md["resp"]
                client.subscribe(mqtt_req)

