#!/usr/bin/env python
# 2014-08-26 PCF8591.py
import time
import curses
import pigpio
import sys


# sudo pigpiod
# ./PCF8591.py

# Connect Pi 3V3 - VCC, Ground - Ground, SDA - SDA, SCL - SCL.

YL_40=0x8
pi = pigpio.pi() # Connect to local Pi.
handle = pi.i2c_open(1, YL_40)

aout = 0

pi.i2c_write_byte_data(handle, 0x41, 0x41)
n,vx = pi.i2c_read_device(handle,16)
#menu1=str(vx.decode())
menu1="" + str(vx).decode() 
print(menu1)

pi.i2c_write_byte_data(handle, 0x41, 0x42)
n,vx = pi.i2c_read_device(handle,16)
#menu1=str(vx.decode())
menu2="" + str(vx).decode() 
print(menu2)

#menu1 =vx[0]
#menu1[1]=vx[1]
#menu1[2]=vx[2]
#menu1[3]=vx[3]
sys.exit(0)


stdscr = curses.initscr()
curses.noecho()
curses.cbreak()
stdscr.addstr(10, 0, menu1)
#stdscr.addstr(10, 0, "Brightness")
stdscr.addstr(12, 0, "Temperature")
stdscr.addstr(14, 0, "AOUT->AIN2")
stdscr.addstr(16, 0, "Resistor")
stdscr.nodelay(1)

try:
    while True:
        for a in range(0,4):
            aout = aout + 1
            #pi.i2c_write_byte_data(handle, 0x40 | ((a+1) & 0x03), 0x32)
            pi.i2c_read_byte_data(handle, 0x50 | ((a+1) & 0x03))
            #v = pi.i2c_read_byte(handle)
            n,vx = pi.i2c_read_device(handle,8)
            v = 21
            hashes = n / 4
            spaces = 64 - hashes
            stdscr.addstr(10+a*2, 12, str(vx[2]) + ' ')
            stdscr.addstr(10+a*2, 16, '#' * hashes + ' ' * spaces )
        stdscr.refresh()
        time.sleep(0.04)
        time.sleep(0.5)
        c = stdscr.getch()
        if c != curses.ERR:
            break
except:
       pass
curses.nocbreak()
curses.echo()
curses.endwin()
#pi.i2c_close(handle)
#pi.stop()

   
