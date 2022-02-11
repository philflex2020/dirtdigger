import time
import board
import busio
import adafruit_ads1x15.ads1015 as ADS
from adafruit_ads1x15.analog_in import AnalogIn
import socket
import json
import platform
import struct
import udplisten as udpl
    
# Create the I2C bus
i2c = busio.I2C(board.SCL, board.SDA)

# Create the ADC object using the I2C bus
ads = ADS.ADS1015(i2c)
chan = []
# Create single-ended input on channel 0
chan.append(AnalogIn(ads, ADS.P0))
chan.append(AnalogIn(ads, ADS.P1))
chan.append(AnalogIn(ads, ADS.P2))
chan.append(AnalogIn(ads, ADS.P3))
#chan1 = AnalogIn(ads, ADS.P1)
#chan2 = AnalogIn(ads, ADS.P2)
#chan3 = AnalogIn(ads, ADS.P3)

# Create differential input between channel 0 and 1
# chan = AnalogIn(ads, ADS.P0, ADS.P1)

print("{:>5}\t{:>5}".format("raw", "v"))

while True:
    dall = {}
    for i  in range(0,4):
        a = "anin" + str(i)
        dall[a] = {}
        dall[a]["value"] = chan[i].value
        dall[a]["volts"] = chan[i].voltage

    msg = udpl.getHostMsg("*", "set", dall, " ")
    for i  in range(0,4):
       print("{:>5} -{:>5}\t{:>5.3f}".format(i, chan[i].value, chan[i].voltage))
    print(msg)
    udpl.sockSend(msg)
    print("\n")
    
    time.sleep(0.5)
