import RPi_I2C_driver
#import I2C_LCD_driver
import socket
import fcntl
import struct
from time import *
import psutil


#mylcd = I2C_LCD_driver.lcd()
mylcd = RPi_I2C_driver.lcd()

def get_cpu_temp():
    foo = psutil.sensors_temperatures()
    for name,entries in foo.items():
        for entry in entries:
            os = (" %-20s %s DegC " %(entry.label or name, entry.current))
            return os
        

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,
        struct.pack('256s', ifname[:15])
    )[20:24])

os = get_cpu_temp()
print os
print os[:12]
print os[21:]

while True:
    mylcd.lcd_display_string("IP Address:     ", 1)
    mylcd.lcd_display_string(get_ip_address('eth0')+"    ", 2)
    sleep(5)
    done = 0
    while done < 5:
        mylcd.lcd_display_string("Time: %s" %strftime("%H:%M:%S"), 1)
        mylcd.lcd_display_string("Date: %s" %strftime("%m/%d/%Y"), 2)
        sleep(1)
        done += 1
    done = 0
    while done < 5:
        os = get_cpu_temp()
        #print os
        #print os[:12]
        #print os[12:]

        mylcd.lcd_display_string(os[:12]+"   ", 1)
        mylcd.lcd_display_string(os[21:]+"   ", 2)
        sleep(1)
        done += 1
 
        
        
while foo:
    # test 2
    mylcd.lcd_display_string("RPi I2C test", 1)
    mylcd.lcd_display_string(" Custom chars", 2)
