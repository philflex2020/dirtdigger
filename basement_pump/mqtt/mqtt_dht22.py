#!/usr/bin/python

# Copyright (c) 2014 Adafruit Industries
# Author: Tony DiCola

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import socket
import fcntl
import struct
import psutil
import time

import Adafruit_DHT
MQTT_SERVER = "127.0.0.1"
MQTT_PATH = "DHT22_channel"

# Sensor should be set to Adafruit_DHT.DHT11,
# Adafruit_DHT.DHT22, or Adafruit_DHT.AM2302.
sensor = Adafruit_DHT.DHT22

# Example using a Beaglebone Black with DHT sensor
# connected to pin P8_11.
#pin = 'P8_11'

# Example using a Raspberry Pi with DHT sensor
# connected to GPIO23.
#pin = 23

# connected to GPIO18.
dht_pin = 18

def dht22_send(path, hum, temp):
    msg = {}
    msg['Temp']= '{0:0.1f}*C'.format(temp)
    msg['Humidity']= '{0:0.1f}%'.format(hum)
    jmsg = json.dumps(msg);
    publish.single(path, jmsg, hostname=MQTT_SERVER) 

# Try to grab a sensor reading.  Use the read_retry method which will retry up
# to 15 times to get a sensor reading (waiting 2 seconds between each retry).
def get_dht22():
    humidity, temperature = Adafruit_DHT.read_retry(sensor, dht_pin)
    return humidity, temperature

# Note that sometimes you won't get a reading and
# the results will be null (because Linux can't
# guarantee the timing of calls to read the sensor).
# If this happens try again!

if __name__ == "__main__":
    while True:
        hum,temp = get_dht22()
    
        if hum is not None and temp is not None:
            print('Temp={0:0.1f}*C  Humidity={1:0.1f}%'.format(temp, hum))
            dht22_send(MQTT_PATH, hum,temp)
        else:
            print('Failed to get reading. Try again!')
