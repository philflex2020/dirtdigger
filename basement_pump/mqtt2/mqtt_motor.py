"""

This version will accept angle , range information from a sensor
It will build up a list of objects from that information 
This version will simply display messages (delivered in json) for the lcd

Test this with 
mosquitto_pub -h localhost -t MOTOR_chan -m '{"cmd":"run","speed":0.5,"heading":370, "steer":-0.3}'

get status with mosquitto_pub -h localhost -t MOTOR_stat -m 
'{"replyto":"phil","reply-topic":"MOTOR_stat"}'

mosquitto_sub -h localhost -v -t "MOTOR_data"


"""
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import socket
import fcntl
import struct
import psutil
#import RPi.GPIO as GPIO
import time
import argparse
import threading 
import math

# originally gpios
# copied to motor
# motor functions are speed ( +/- or 0)
# motor functions are pos ( +/- or 0)
# motor has a dir angle 
# if pos = 0
# dist = (speed * time)
# xinc = dist * sin(angle)
# yinc = dist * cos(angle)
# xpos += xinc
# ypos += yinc
# 
#MQTT_SERVER = "127.0.0.1"
MQTT_SERVER = "localhost"
MOTOR_CHAN = "MOTOR_chan"
MOTOR_STAT = "MOTOR_stat"
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
#GPIO.setmode(GPIO.BCM)
#GPIO.setwarnings(False)
#GPIO.setup(chan25, GPIO.OUT)
#GPIO.setup(chan23, GPIO.OUT)
#GPIO.setup(chan17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
#GPIO.setup(chan22, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
#pwm = GPIO.PWM(chan25,1000)
pwmd = 10
padd = 20
#pwm.start(pwmd)
oldvals = {}

gmotor = {}

gmotor["run"]= 1
gmotor["speed"] = 0
gmotor["steer"] = 0
gmotor["heading"] = 0.0
gmotor["xpos"] = 0.0
gmotor["ypos"] = 0.0
gmotor["dist"]  = 0

def switch_send(path, chan1, val1, chan2, val2):
    msg = {}
    msg['key']= "switch"
    msg['chan1']= chan1
    msg['chan2']= chan2
    msg['val1']= val1
    msg['val2']= val2
    jmsg = json.dumps(msg);
    publish.single(path, jmsg, hostname=MQTT_SERVER)

def motor_send(path, chan, value, host=MQTT_SERVER):
    global gmotor
    msg = {}
    msg['key']= "switch"
    msg['chan']= chan
    msg['value']= value
    jmsg = json.dumps(gmotor);
    print ("jmsg :" + jmsg)
    try:
        publish.single('MOTOR_data', jmsg, hostname=host)
    except:
        print (" publish failed")

gvalues = {}

def getGval(chan):
    global gvalues
    if chan in gvalues.keys():
        cval = gvalues[chan]
    else:
        gvalues[chan] = 0;

    gvalues[chan]  += 1
    return gvalues[chan]

def setGval(chan, val):
    global gvalues
    #if chan in gvalues.keys():
    #    cval = gvalues[chan]
    #else:
    gvalues[chan] = val;

    return 

        
def my_callback_one(chan):
    global oldvals
    #value = GPIO.input(chan)
    value = getGval(chan)
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
        #motor_send(SW_PATH, chan, value)


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
    client.subscribe(MOTOR_CHAN)
    print("Subscribed to " + MOTOR_CHAN)
    client.subscribe(MOTOR_STAT)
    print("Subscribed to " + MOTOR_STAT)

#grun = 1
#gspeed = 0
#gsteer = 0
#gheading = 0.0
#gxpos = 0.0
#gypos = 0.0
#gdist  = 0
gsendit = 0

 # The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global gmotor
    #global gspeed
    #global gsteer
    #global gheading
    global gsendit
    
    if msg.topic == "MOTOR_chan":
        print(msg.topic+" MOTOR->"+str(msg.payload))
        jmes = json.loads(msg.payload)
        if "pin" in jmes.keys():
            if jmes["pin"] == chan18:
                if jmes["value"] == "on":
                    #GPIO.output(chan18, GPIO.HIGH)
                    setGval(chan18, 1)
                else:
                    #GPIO.output(chan18, GPIO.LOW)
                    setGval(chan18, 0)

            if jmes["pin"] == chan23:
                if jmes["value"] == "on":
                    #GPIO.output(chan23, GPIO.HIGH)
                    setGval(chan23, 1)
                else:
                    #GPIO.output(chan23, GPIO.LOW)
                    setGval(chan23, 0)
        if "cmd" in jmes.keys():
            if jmes["cmd"] == "stop":
                gmotor["run"]  = 0;
            if jmes["cmd"] == "run":
                gmotor["run"]  = 1;
        if "speed" in jmes.keys():
            gmotor["speed"] = jmes["speed"]
        if "steer" in jmes.keys():
            gmotor["steer"] = jmes["steer"]
            print (" set steer :"+ str(gsteer)) 
        if "heading" in jmes.keys():
            gmotor["heading"] = jmes["heading"]
            print (" set heading :"+ str(gmotor["heading"])) 
            print (" set heading rad :"+ str(math.radians(gmotor["heading"]))) 
    if msg.topic == "MOTOR_stat":
        print(msg.topic+" STAT->"+str(msg.payload))
        jmes = json.loads(msg.payload)
        print("not running motor_send\n")
        gsendit+=1
        #motor_send(reply_topic, "chan", "value", replyto)
        print("done not running motor_send\n")
        #if "replyto" in jmes.keys():
        #    replyto = jmes["replyto"]
        #    if "reply-topic" in jmes.keys():
        #        reply_topic = jmes["reply-topic"]

    #mylcd.lcd_display_string(jmes["line1"],1)
    #mylcd.lcd_display_string(jmes["line2"],2)
    # more callbacks, etc



def motorThread(val, stop):
    global gmotor
    #global gdist
    #global gspeed
    #global gxpos
    #global gypos
    #global gheading
    global gsendit
    
    oldval={}
    ticks = 0
    print (" thread running ")
    while True:
        if gsendit > 0:
            print("running motor_send gsendit: " +str(gsendit))
            motor_send("MOTOR_data", "chan", "value", "localhost")
            print("after running motor_send")
            #gsendit = 0

        ticks += 1
        if gmotor["run"] == 1:
            gmotor["dist"] = gmotor["speed"]
            gmotor["heading"] += gmotor["steer"]
            xinc = gmotor["dist"] * math.sin(math.radians(gmotor["heading"]))
            yinc = gmotor["dist"] * math.cos(math.radians(gmotor["heading"]))
            print ("***************")
            print (" heading :"+ str(gmotor["heading"])) 
            print (" rads: " + str(math.radians(gmotor["heading"])))
            print (" gdist: " + str(gmotor["dist"]) +  " sin: " + str(math.sin(math.radians(gmotor["heading"]))))

            print (" xinc : " + str(xinc))
            print (" yinc : " + str(yinc))
            gmotor["xpos"] +=xinc
            gmotor["ypos"] +=yinc
            
            
        print (" ticks: " + str(ticks) + " x,y: " + str(round(gmotor["xpos"],2)) + "," + str(round(gmotor["ypos"],2)) )

        if stop():
            print " thread exit"
            break
        if gmotor["run"] == 1:
            for chan in (chan17, chan22):
        
                #value = GPIO.input(chan)
                value = getGval(chan)
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

def setupJTest(val, txt):        
    jTest = {}

    jTest["val"] = val
    jTest["txt"] = txt
    return jTest
        
        
if __name__ == "__main__":
    #parser = argparse.ArgumentParser()
    #parser.add_argument('--serv', help="Server to connect [localhost]")
    #args = parser.parse_args()
    stop_thread = False
    jj = setupJTest(21,"thisisatest")
    jmsg = json.dumps(jj);
    print(" we got jj [ " + jmsg +"]")
    
    t = threading.Thread(target=motorThread, args=(1,lambda: stop_thread))
    t.start()

    #time.sleep(5.0)
    #stop_thread = True
    #t.join()
    print(" starting stuff\n")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, 1883, 60)
    print client

    try:
        #client = mqtt.Client()
        #client.on_connect = on_connect
        #client.on_message = on_message
        #client.connect(MQTT_SERVER, 1883, 60)
        
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
    
