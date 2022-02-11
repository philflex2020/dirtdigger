import time
import board
import busio
#import adafruit_ads1x15.ads1015 as ADS
#from adafruit_ads1x15.analog_in import AnalogIn
import ADS1X15 as ADS
#from ADS1X15.analog_in import AnalogIn
import socket
import json
import platform
import struct
import udpsystem as udps

# Create the I2C bus
#i2c = busio.I2C(board.SCL, board.SDA)

# Create the ADC object using the I2C bus
#ads = ADS.ADS1115(i2c)
adc = ADS.ADS1115(0x48)
#chan = []
# Create single-ended input on channel 0
#chan.append(AnalogIn(ads, ADS.P0))
#chan.append(AnalogIn(ads, ADS.P1))
#chan.append(AnalogIn(ads, ADS.P2))
#chan.append(AnalogIn(ads, ADS.P3))
#chan.append(0)
#chan.append(1)
#chan.append(2)
#chan.append(3)
#chan1 = AnalogIn(ads, ADS.P1)
#chan2 = AnalogIn(ads, ADS.P2)
#chan3 = AnalogIn(ads, ADS.P3)

# Create differential input between channel 0 and 1
# chan = AnalogIn(ads, ADS.P0, ADS.P1)

print("{:>5}\t{:>5}".format("raw", "v"))

scan_time = 0.5
threadRun = True
g_rC = {}

def adcScan():
    #global chan
    global adc
    global g_rC
    rC = {}
    for i  in range(0,4):
        a = "anin" + str(i)
        rC[a] = {}
        #rC[a]["value"] = chan[i].value
        if a in g_rC.keys() :
            if "value" in g_rC[a].keys():
                rC[a]["value"] = g_rC[a]["value"]
                rC[a]["volts"] = (rC[a]["value"] * 3.65) / 0x448a
    msg =  ""
    for i  in range(0,4):
        a = "anin" + str(i)
        if "value"  in rC[a].keys():
            msg += " hex:" + hex(rC[a]["value"])
            #print (msg)
        
        #print("{:>5} -{:>5}\t{:>5.3f}".format(i, rC[a]["value"], (rC[a]["value"] * 3.65) / 0x448a ))
    print(msg)                                          
    return rC

# generates an event when inputs change
g_adcFirst = 1;


def adcIn(key):

    global adc
    global g_adcFirst
    global g_rC

    sendEvent = 0
    if g_adcFirst == 1:
        for i  in range(0,4):
            a = "anin" + str(i)
            g_rC[a] = {}
            #rC[a]["value"] = chan[i].value
            g_rC[a]["value"] = adc.read_adc(i,gain=2/3)
            g_rC[a]["lvalue"] = g_rC[a]["value"]
        g_adcFirst = 0
    else:
        for i  in range(0,4):
            a = "anin" + str(i)
            #rC[a]["value"] = chan[i].value
            g_rC[a]["value"] = adc.read_adc(i,gain=2/3)

            g_rC[a]["ldiff"] = g_rC[a]["lvalue"] - g_rC[a]["value"]
            ldiff = g_rC[a]["ldiff"]
            if ldiff < -50 or ldiff > 50:
                sendEvent = 1

         
    
    if sendEvent == 1:
        rC = {}
        for i  in range(0,4):
            
            a = "anin" + str(i)
            rC[a] = {}
            rC[a]["value"] = g_rC[a]["value"] 
            rC[a]["ldiff"] = g_rC[a]["ldiff"] 

        udps.sendEvent("*","event","adc1In", rC)

    for i  in range(0,4):
        a = "anin" + str(i)
        #msg = " hex:" + hex(rC[a]["value"])
        #print (msg)
        
        g_rC[a]["lvalue"] = g_rC[a]["value"]
        
    return g_rC
    
if __name__ == "__main__":
    udps.runTask("AdcIn",adcIn,0.05, None)
    udps.runService(None,["adc1",adcScan], 5678, 1)
