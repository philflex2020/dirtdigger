import time
import I2C_LCD_driver
import socket
import fcntl
import struct
import udpsystem as udps

mylcd = I2C_LCD_driver.lcd()

lcds1 = ""
lcds2 = ""
lcds3 = ""
lcds4 = ""

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915, 
        struct.pack('256s', ifname[:15])
    )[20:24])

def lcdOut(data):
    mylcd.lcd_display_string(lcds1, 1)
    mylcd.lcd_display_string(lcds2, 2)     

udps.runTask("LcdOut", lcdOut,1, None);

while True:
    #mylcd.lcd_display_string(u" Hello LCD",0)
    #time.sleep(1)
    #mylcd.lcd_clear()
    time.sleep(1)
    s1 = "%s" %time.strftime("%H:%M:%S")
    s2 = "%s" %time.strftime("%m/%d/%Y")
    lcds1 = s2 + ":" + s1
    lcds2 = "IP:" + get_ip_address(b'eth0') 


