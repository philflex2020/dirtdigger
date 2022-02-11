#!/usr/bin/env python
# display stuff as an i2c slave
"""

This code is an example of using the Pi as an I2C slave device

receiving messages from an Arduino acting as the I2C bus master.



On the Pi



The BSC slave SDA is GPIO 18 (pin 12) and the BSC slave

SCL is GPIO 19 (pin 35).



On the Arduino



SDA is generally A4 and SCL is generally A5.



Make the following connections.



GPIO 18 <--- SDA ---> A4

GPIO 19 <--- SCL ---> A5



You should also add pull-up resistors of 4k7 or so on each of

GPIO 18 and 19 to 3V3.  The software sets the internal pull-ups

which may work reliably enough.



On the Arduino use the following code.



#include <Wire.h>
void setup()
{
   Wire.begin(); // join i2c bus as master
}

char str[17];
int x = 0;

void loop()
{
   sprintf(str, "Message %7d\n", x);
   if (++x > 9999999) x=0;
   Wire.beginTransmission(9); // transmit to device #9
   Wire.write(str);           // sends 16 bytes
   Wire.endTransmission();    // stop transmitting
   Wire.requestFrom(9,8);
   while (Wire.available()) {
     char c = Wire.read();
     Serial.print(c);
   }
   Serial.println(str);
   delay(50);
}
"""




import time
import curses
import pigpio
import sys


# sudo pigpiod
# ./PCF8591.py

# Connect Pi 3V3 - VCC, Ground - Ground, SDA - SDA, SCL - SCL.
I2C_ADDR=9




SDA=18
SCL=19
I2C_ADDR=9



def i2c(id, tick):
    global pi
    print("i2c")
    s, b, d = pi.bsc_i2c(I2C_ADDR)
    if b:
        print(d[:-1])

pi = pigpio.pi()
if not pi.connected:
    exit()

# Add pull-ups in case external pull-ups haven't been added
pi.set_pull_up_down(SDA, pigpio.PUD_UP)
pi.set_pull_up_down(SCL, pigpio.PUD_UP)


print("i2c starting")

# Respond to BSC slave activity
e = pi.event_callback(pigpio.EVENT_BSC, i2c)
pi.bsc_i2c(I2C_ADDR) # Configure BSC as I2C slave
print("i2c waiting")

time.sleep(5)
print("i2c done")
e.cancel()
pi.bsc_i2c(I2C_ADDR) # Disable BSC peripheral
pi.stop()


                               
#YL_40=0x8
#pi = pigpio.pi() # Connect to local Pi.
#handle = pi.i2c_open(1, YL_40)

#aout = 0

#pi.i2c_write_byte_data(handle, 0x41, 0x41)
#n,vx = pi.i2c_read_device(handle,16)
#menu1=str(vx.decode())
#menu1="" + str(vx).decode() 
#print(menu1)

#pi.i2c_write_byte_data(handle, 0x41, 0x42)
#n,vx = pi.i2c_read_device(handle,16)
#menu1=str(vx.decode())
#menu2="" + str(vx).decode() 
#print(menu2)

#menu1 =vx[0]
#menu1[1]=vx[1]
#menu1[2]=vx[2]
#menu1[3]=vx[3]
#sys.exit(0)


#stdscr = curses.initscr()
#curses.noecho()
#curses.cbreak()
#stdscr.addstr(10, 0, menu1)
#stdscr.addstr(10, 0, "Brightness")
#stdscr.addstr(12, 0, "Temperature")
#stdscr.addstr(14, 0, "AOUT->AIN2")
#stdscr.addstr(16, 0, "Resistor")
#stdscr.nodelay(1)

#try:
#    while True:
#        for a in range(0,4):
#            aout = aout + 1
#            #pi.i2c_write_byte_data(handle, 0x40 | ((a+1) & 0x03), 0x32)
#            pi.i2c_read_byte_data(handle, 0x50 | ((a+1) & 0x03))
#            #v = pi.i2c_read_byte(handle)
#            n,vx = pi.i2c_read_device(handle,8)
#            v = 21
#            hashes = n / 4
#            spaces = 64 - hashes
#            stdscr.addstr(10+a*2, 12, str(vx[2]) + ' ')
#            stdscr.addstr(10+a*2, 16, '#' * hashes + ' ' * spaces )
#        stdscr.refresh()
#        time.sleep(0.04)
#        time.sleep(0.5)
#        c = stdscr.getch()
#        if c != curses.ERR:
#            break
#except:
#       pass
#curses.nocbreak()
#curses.echo()
#curses.endwin()
#pi.i2c_close(handle)
#pi.stop()

   
