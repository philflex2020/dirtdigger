#mosquitto_pub -h localhost -t test_channel -m "Hello Raspberry Pi
"""

http://www.gpsinformation.org/dale/nmea.htm

GGA - essential fix data which provide 3D location and accuracy data.

 $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
			       4 = Real Time Kinematic
			       5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
			       7 = Manual input mode
			       8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
     *47          the checksum data, always begins with *


 $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39

Where:
     GSA      Satellite status
     A        Auto selection of 2D or 3D fix (M = manual) 
     3        3D fix - values include: 1 = no fix
                                       2 = 2D fix
                                       3 = 3D fix
     04,05... PRNs of satellites used for fix (space for 12) 
     2.5      PDOP (dilution of precision) 
     1.3      Horizontal dilution of precision (HDOP) 
     2.1      Vertical dilution of precision (VDOP)
     *39      the checksum data, always begins with *

 $GPGSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45*75

Where:
      GSV          Satellites in view
      2            Number of sentences for full data
      1            sentence 1 of 2
      08           Number of satellites in view

      01           Satellite PRN number
      40           Elevation, degrees
      083          Azimuth, degrees
      46           SNR - higher is better
           for up to 4 satellites per sentence
      *75          the checksum data, always begins with *

$GPRMC,115833.400,A,3545.7539,N,07852.2982,W,0.00,242.10,131099,,,A*75

$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
     *6A          The checksum data, always begins with *

GLL - Geographic Latitude and Longitude is a holdover from Loran data and some old units may not send the time and data active information if they are emulating Loran data. If a gps is emulating Loran data they may use the LC Loran prefix instead of GP.

  $GPGLL,4916.45,N,12311.12,W,225444,A,*1D

Where:
     GLL          Geographic position, Latitude and Longitude
     4916.46,N    Latitude 49 deg. 16.45 min. North
     12311.12,W   Longitude 123 deg. 11.12 min. West
     225444       Fix taken at 22:54:44 UTC
     A            Data Active or V (void)
     *iD          checksum data

VTG - Velocity made good. The gps receiver may use the LC prefix instead of GP if it is emulating Loran output.

  $GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48

where:
        VTG          Track made good and ground speed
        054.7,T      True track made good (degrees)
        034.4,M      Magnetic track made good
        005.5,N      Ground speed, knots
        010.2,K      Ground speed, Kilometers per hour
        *48          Checksum

Typical results
o satellite data available
raw: $GPVTG,0.00,T,,M,0.00,N,0.00,K,N*32
raw: $GPGGA,114135.000,3740.0395,N,08130.8112,W,0,0,,182.9,M,-32.9,M,,*7A
raw: $GPGLL,3740.0395,N,08130.8112,W,114135.000,V,N*5F
raw: $GPGSA,A,1,,,,,,,,,,,,,,,*1E
raw: $GPGSV,1,1,00*79
raw: $GPRMC,114135.000,V,3740.0395,N,08130.8112,W,0.00,0.00,121099,,,N*6A
no satellite data available


sudo pip install geopy

See also 
https://geopy.readthedocs.io/en/stable/

"""

import serial
import geopy.distance
from geopy.geocoders import Nominatim

import math, numpy as np



port = "/dev/ttyUSB0"

lastc = (0,0)
lastc1 = (0,0)

def parseGPS(data):
    global lastc
    global lastc1
    #print "raw:", data #prints raw data
    if data[0:6] == "$GPRMC":
        sdata = data.split(",")
        if sdata[2] == 'V':
            print "no satellite data available"
            print "raw:", data #prints raw data
            return
        print "---Parsing GPRMC---", data,
        time = sdata[1][0:2] + ":" + sdata[1][2:4] + ":" + sdata[1][4:6]
        lat = decode(sdata[3]) #latitude
        lat2 = decode2(sdata[3]) #latitude
        latm = min2(sdata[3]) #latitude
        dirLat = sdata[4]      #latitude direction N/S
        lon = decode(sdata[5]) #longitute
        lon2 = decode2(sdata[5]) #longitute
        lonm = min2(sdata[5]) #longitute
        dirLon = sdata[6]      #longitude direction E/W
        speed = sdata[7]       #Speed in knots
        trCourse = sdata[8]    #True course
        date = sdata[9][0:2] + "/" + sdata[9][2:4] + "/" + sdata[9][4:6]#date
        lastc1 = lastc
        lastc = (latm,lonm)
 
        print "time : %s, latitude : %s(%s), longitude : %s(%s), speed : %s, True Course : %s, Date : %s" %  (time,lat,dirLat,lon,dirLon,speed,trCourse,date)
        print " lat2 %s lon2 %s" % (lat2,lon2)
        print " distance %f " % dist2(lastc1, lastc)

def decode(coord):
    #Converts DDDMM.MMMMM > DD deg MM.MMMMM min
    x = coord.split(".")
    head = x[0]
    tail = x[1]
    deg = head[0:-2]
    min = head[-2:]
    return deg + " deg " + min + "." + tail + " min"

def min2(coord):
    #Converts DDDMM.MMMMM > DD.dddddd  
    x = coord.split(".")
    head = x[0]
    tail = x[1]
    deg = head[0:-2]
    min = head[-2:]
    mina = min +"."+tail
    #mina
    min = float(deg) +(float(mina))/60.0
    return min

def decode2(coord):
    min = min2(coord)
    return "%02.6f" % min

def dist2(c1, c2):
    return geopy.distance.distance(c1,c2).feet

def get_bearing(c1, c2):
    lat1 = np.deg2rad(c1[0])
    lon1 = np.deg2rad(c1[1])
    lat2 = np.deg2rad(c2[0])
    lon3 = np.deg2rad(c2[1])
    dLon = lon2 - lon1
    y = math.sin(dLon) * math.cos(lat2)
    x = math.cos(lat1) * math.sin(lat2) - math.sin(lat1)*math.cos(lat2)*math.cos(dLon)
    brng = np.rad2deg(math.atan2(y,x))
    if brng < 0 : brng += 360
    return brng

def get_latlon (c1, dist, brng):
    R = 6378.137 #radius of the earth at equator
    #R = 6356.752 #radius of the earth at pole
    
    brng = np.deg2rad(brng)
    lat1 = np.deg2rad(c1[0])
    lon1 = np.deg2rad(c1[2])
    lat2 = math.asin( math.sin(lat1)*math.cos(dist/R) +
                      math.cos(lat1)*math.sin(dist/R)*math.cos(brng))
    lon2 = lon1 + math.atan2(math.sin(brng)*math.sin(dist/R)*math.cos(lat1),
                             math.cos(dist/R) - math.sin(lat1)*math.sin(lat2))
    lat2 = np.rad2deg(lat2)
    lon2 = np.rad2deg(lon2)
    return (lat2,lon2)

def getAddress(c1):
    geolocator = Nominatim(user_agent="someuseragent")
    location1 = geolocator.geocode("2009 Passaic Way, Apex, NC , USA") 
    location2 = geolocator.reverse( c1) 
    print (location1.address)
    print (location2.address)
    print (location2.latitude, location2.longitude)
    print (location2.raw)
    



 
print "Receiving GPS data"
getAddress((35.762330,-78.871615))
ser = serial.Serial(port, baudrate = 9600, timeout = 0.5)
while True:
   data = ser.readline()
   parseGPS(data)
