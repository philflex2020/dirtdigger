#!/usr/bin/python
"""
collect the cpu temp from a pine64 board
send data via mqtt to localhost

"""
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import time

# test with mosquitto_sub -h localhost -v -t "pine64-002/#"
MQTT_SERVER = "localhost"
TEMP_CHAN = "pine64-002/cpu_temp"
HOSTNAME = "pine64-002"


degree_sign = u'\N{DEGREE SIGN}'.encode("utf-8")
#tempSource = "/sys/devices/platform/sunxi-i2c.0/i2c-0/0-0034/temp1_input"  # cubietruck
gmsg = {}

def temp_send(topic, msgd):
    jmsg = json.dumps(msgd)
    publish.single(topic, jmsg, hostname=MQTT_SERVER)
    print (jmsg)
    
while True:
    tempSource = "/sys/devices/virtual/thermal/thermal_zone0/temp"  # pine64

    f = open(tempSource, "r")
    cpu_temp = int(f.read())
    f.close()
    gmsg["host"]= HOSTNAME
    gmsg["cpu_temp"]=cpu_temp
    temp_send(TEMP_CHAN,gmsg)
    
    print str(cpu_temp) + degree_sign + 'C'
    time.sleep(1)
