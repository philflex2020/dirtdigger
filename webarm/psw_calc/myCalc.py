"""

Given l1 and l2 calc the range of extensions D we can produce
We also need the angles of each joint

Start with d1 and d2


D can range from l1-l2 to l1+l2

d2 = ((l2*l2) - (l1*l1) - (D * D))/ 2D
d1 = ((l1*l1) - (l2*l2) - (D * D))/ 2D


"""

import numpy as np
import math

l1 = 115
l2 = 68
dArray = np.arange(l1-l2,l1+l2, 0.2)
print(dArray)
print(dArray.size)

for i in dArray:
    d1 = (-(l2*l2) + (l1*l1) + (i * i)) / (2 * i) 
    d2 = (-(l1*l1) + (l2*l2) + (i * i)) / (2 * i) 
    d1int = (int)((d1 *100) % 100) / 100.0
    d2int = (int)((d2 *100) % 100) / 100.0
    Dint = (int)((i *100) % 100) / 100.0
    a1 = math.degrees(np.arccos(d1/l1))
    a2 = math.degrees(np.arccos(d2/l2))
    ax1 = 180 -90 - a1
    ax2 = 180 -90 - a2
    ax = ax1 + ax2
    Dint = i
    print "D=",Dint, " d1=", d1, " d2=", d2 , "a1=" , a1, "ax=" , ax
    
