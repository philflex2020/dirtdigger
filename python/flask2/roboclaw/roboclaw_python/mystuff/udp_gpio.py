#!/usr/bin/python3

import RPi.GPIO as GPIO
import time
import threading
import udpsystem as udps

#W
led = 26
switch = 31

gpouts = []
gpins = []
gpnames = {}
lock = threading.Lock()

#GPIO.setmode(GPIO.BOARD)
GPIO.setmode(GPIO.BCM)
#GPIO.setup(led, GPIO.OUT)

#GPIO.setup(switch, GPIO.IN)

def getGpios():
    rC = {}
    lock.acquire()
    try:
        for pin in gpins:
            ss = "gpin" + str(pin)
            rC[ss] = GPIO.input(pin)
        for pin in gpouts:
            ss = "gpout" + str(pin)
            rC[ss] = GPIO.input(pin)
    finally:
        lock.release()
    return rC


def gpioLoop(arg=led):
    GPIO.output(arg, GPIO.HIGH != GPIO.input(arg))
    

# todo need locks with thread
def gpin(cmds, data, dest):
    if len(cmds) > 2:
        lock.acquire()
        try:
            pin = int(cmds[1])
            if pin not in gpins:
                #GPIO.setup(led, GPIO.OUT)
                GPIO.setup(pin, GPIO.IN)
                gpins.append(pin)
                gpnames[pin] = cmds[2]
            if pin in gpouts:
                gpouts.remove(pin)
        finally:
            lock.release()
            # todo need locks with thread
def gpout(cmds, data, dest):
    if len(cmds) > 2:
        lock.acquire()
        try:
            pin = int(cmds[1])
            if pin not in gpouts:
                GPIO.setup(pin, GPIO.OUT)
                #GPIO.setup(switch, GPIO.IN)
                gpouts.append(pin)
                gpnames[pin] = cmds[2]
            if pin in gpins:
                gpins.remove(pin)
        finally:
            lock.release()
    

if __name__ == "__main__":
    udps.addMenuItem("gpin", "gpio input", " gpin pin", gpin, None)
    udps.addMenuItem("gpout", "gpio output", " gpout pin", gpout, None)
    udps.runMenuInput(udps.udpMenu,"gpout 26 testout")
    udps.runService(None,["gpios",getGpios, gpioLoop], 5676)
    GPIO.cleanup()
