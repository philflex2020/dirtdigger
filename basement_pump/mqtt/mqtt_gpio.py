import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import socket
import fcntl
import struct
import psutil
import RPi.GPIO as GPIO
import time
import argparse
import threading 

MQTT_SERVER = "127.0.0.1"
GPIO_PATH = "GPIO_out"
SW_PATH = "SWITCH_in"

#pin 11  input
chan17 = 17
#pin 22  
chan25 = 25
#pin 13  input
chan22 = 27
#pin 16
chan23 = 23
#pin 
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(chan25, GPIO.OUT)
GPIO.setup(chan23, GPIO.OUT)
GPIO.setup(chan17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(chan22, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
#pwm = GPIO.PWM(chan25,1000)
pwmd = 10
padd = 20
#pwm.start(pwmd)
oldvals = {}


def switch_send(path, chan1, val1, chan2, val2):
    msg = {}
    msg['key']= "switch"
    msg['chan1']= chan1
    msg['chan2']= chan2
    msg['val1']= val1
    msg['val2']= val2
    jmsg = json.dumps(msg);
    publish.single(path, jmsg, hostname=MQTT_SERVER)

def gpio_send(path, chan, value):
    msg = {}
    msg['key']= "switch"
    msg['chan']= chan
    msg['value']= value
    jmsg = json.dumps(msg);
    publish.single(path, jmsg, hostname=MQTT_SERVER)

    
def my_callback_one(chan):
    global oldvals
    value = GPIO.input(chan)
    old=-1
    if chan in oldvals.keys():
        old = oldvals[chan]
        
    print 'Callback chan ' + str(chan) + " val "+ str(value) + " oldval "+ str(old) 
    # todo set display to mode 1
    # if GPIO.input(chan1) or GPIO.input(chan2):
    #  stay in mode 1
    #
    sendit = True
    if chan in oldvals.keys():
        if oldvals[chan] == value:
            sendit = False
    oldvals[chan] = value
    if sendit:
        print "sent"
        gpio_send(SW_PATH, chan, value)


def my_callback_two(channel):
    print('Callback two LCD MOde 2')
    # todo set display to mode 2



#GPIO.add_event_detect(chan17, GPIO.BOTH)
#GPIO.add_event_detect(chan22, GPIO.BOTH)
#GPIO.add_event_callback(chan17, my_callback_one)#, bouncetime=200)
#GPIO.add_event_callback(chan22, my_callback_one)#, bouncetime=200)

    
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe(MQTT_PATH1)
    client.subscribe(GPIO_PATH)

 # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" GPIO->"+str(msg.payload))
    jmes = json.loads(msg.payload)
    if jmes["pin"] == chan18:
        if jmes["value"] == "on":
            GPIO.output(chan18, GPIO.HIGH)
        else:
            GPIO.output(chan18, GPIO.LOW)

    if jmes["pin"] == chan23:
        if jmes["value"] == "on":
            GPIO.output(chan23, GPIO.HIGH)
        else:
            GPIO.output(chan23, GPIO.LOW)
            
    #mylcd.lcd_display_string(jmes["line1"],1)
    #mylcd.lcd_display_string(jmes["line2"],2)
    # more callbacks, etc

def gpioThread(val, stop):
    oldval={}
    ticks = 0
    while True:
        ticks += 1
        if stop():
            print " thread exit"
            break

        for chan in (chan17, chan22):
        
            value = GPIO.input(chan)
            old=-1
            if chan in oldval.keys():
                old = oldval[chan]
        
            
            oldval[chan] = value
            print 'Thread  tick ' + str(ticks) +' chan ' + str(chan) + " val "+ str(value) + " oldval "+ str(old) 
            # todo set display to mode 1
            # if GPIO.input(chan1) or GPIO.input(chan2):
            #  stay in mode 1
            #
            sendit = True
            if old  == value:
                sendit = False
            if sendit:
                print "sent"
                gpio_send(SW_PATH, chan, value)
            
        time.sleep(1.0)
    

if __name__ == "__main__":
    #parser = argparse.ArgumentParser()
    #parser.add_argument('--serv', help="Server to connect [localhost]")
    #args = parser.parse_args()
    stop_thread = False
    
    t = threading.Thread(target=gpioThread, args=(1,lambda: stop_thread))
    t.start()

    #time.sleep(5.0)
    #stop_thread = True
    #t.join()
    try:
        client = mqtt.Client()
        client.on_connect = on_connect
        client.on_message = on_message
        client.connect(MQTT_SERVER, 1883, 60)
        
        # Blocking call that processes network traffic, dispatches callbacks and
        # handles reconnecting.
        
        # Other loop*() functions are available that give a threaded interface and a
        # manual interface.
        client.loop_forever()
    except KeyboardInterrupt:
        print 'quitting'
        stop_thread = True
        t.join()   
        
#    while True:
#        pwmd +=padd
#        if pwmd >= 80:
#            padd = -20
#        if pwmd < 20:
#            padd = 20
                
#        #pwm.ChangeDutyCycle(pwmd)
#        #GPIO.output(chan18, GPIO.HIGH)
#        GPIO.output(chan23, GPIO.LOW)
#        print GPIO.input(chan17)
#        time.sleep(1)
#        GPIO.output(chan23, GPIO.HIGH)
#        #GPIO.output(chan18, GPIO.LOW)
#        time.sleep(1)

#t.stop()
    
