#!/usr/bin/python3
#deg_sign = u'\N{DEGREE_SIGN}'.encode('utf8')
tmp_src = '/sys/devices/virtual/thermal/thermal_zone0/temp'

f = open(tmp_src,"r")
cpu_temp = int(f.read())
f.close()

print (str(cpu_temp) + ' ' + 'C')
