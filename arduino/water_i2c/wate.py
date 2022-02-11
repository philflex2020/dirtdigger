import pigpio
pi = pigpio.pi()
print pi.connected
#True

h = pi.i2c_open(1,8)
pi.i2c_write_byte_data(h,0x41,0x32)
pi.i2c_close(h)
pi.stop()
