# Simple demo of of the PCA9685 PWM servo/LED controller library.
# This will move channel 0 from min to max position repeatedly.
# Author: Tony DiCola
# License: Public Domain
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

get status with mosquitto_pub -h localhost -t MOTOR_stat -m 
'{"replyto":"phil","reply-topic":"MOTOR_stat"}'

mosquitto_sub -h localhost -v -t "MOTOR_data"


"""
from __future__ import division
import time
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import sys
# Import the PCA9685 module.
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
servo_min = 150  # Min pulse length out of 4096
servo_max = 600  # Max pulse length out of 4096

g_vals = {}

# Helper function to make setting a servo pulse width simpler.
def set_servo_pulse(channel, pulse):
    pulse_length = 1000000    # 1,000,000 us per second
    pulse_length //= 60       # 60 Hz
    print('{0}us per period'.format(pulse_length))
    pulse_length //= 4096     # 12 bits of resolution
    print('{0}us per bit'.format(pulse_length))
    pulse *= 1000
    pulse //= pulse_length
    pwm.set_pwm(channel, 0, pulse)


def servo_test(chan): 
    # Set frequency to 60hz, good for servos.


    print('Moving servo on channel '+str(chan)+', press Ctrl-C to quit...')
    while True:
        # Move servo on channel O between extremes.
        print "max"
        pwm.set_pwm(chan, 0, servo_min)
        time.sleep(1)
        print "min"
        pwm.set_pwm(chan, 0, servo_max)
        time.sleep(1)

MQTT_SERVER = "localhost"
SERVO_DRIVE = "/servo/drive"
SERVO_DATA = "/servo/data"
      
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(MQTT_PATH1)
    client.subscribe(SERVO_DRIVE)
    print("Subscribed to " + SERVO_DRIVE)
    client.subscribe(SERVO_DATA)
    print("Subscribed to " + SERVO_DATA)

# '{"data":[{<servo_num>,<pulse>},]}'
# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global g_vals
    #global gspeed
    #global gsteer
    #global gheading
    #global gsendit
    
    if msg.topic == SERVO_DRIVE:
        print(msg.topic+" SERVO->"+str(msg.payload) + "<<")
        jmes = json.loads(msg.payload)
        
        print jmes
        if "data" in jmes.keys():
            vals =jmes["data"]
            print vals
            for val in vals.keys():
                print "servo : " + val + " pulse :" + str(vals[val])
                #print " servo "+str(val[0]) + "pulse " + str(val[1])
                g_vals[val] = vals[val]
                snum=int(val)
                pwm.set_pwm(snum, 0, vals[val])
                #set_servo_pulse(snum, vals[val])
                
                


    
if __name__ == "__main__":
    servo = -1;
    print sys.argv
    if len(sys.argv) > 1:
        servo = int(sys.argv[1])
    print "servo = "+ str(servo)
    #t = threading.Thread(target=motorThread, args=(1,lambda: stop_thread))
    #t.start()

    #time.sleep(5.0)
    #stop_thread = True
    #t.join()
    if servo > -1:
        servo_test(servo)
    
    print(" starting mqtt stuff\n")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, 1883, 60)
    print client
    client.loop_forever() 
