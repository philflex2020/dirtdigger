        
#import FaBo9Axis_MPU9250
import myMPU9250 as mpu
import time
import sys

#mpu9250 = FaBo9Axis_MPU9250.MPU9250()
mpu9250 = mpu.myMPU9250()

try:
    while True:
        accel = mpu9250.readAccel()
        print (" ax = "  + str( accel['x'] ))
        print (" ay = " + str( accel['y'] ))
        print (" az = "+ str( accel['z'] ))

        gyro = mpu9250.readGyro()
        print (" gx = " +str(gyro['x'])) 
        print (" gy = "  + str( gyro['y'] ))
        print (" gz = " + str(gyro['z'] ))

        mag = mpu9250.readMagnet()
        print (" mx = "  + str( mag['x'] ))
        print (" my = " + str( mag['y'] ))
        print (" mz = " + str ( mag['z'] ))
        
        temp = mpu9250.readTemperature()
        print ("temp = "+ str(temp))
        print ("\n")
        

        time.sleep(0.5)

except KeyboardInterrupt:
    sys.exit()
