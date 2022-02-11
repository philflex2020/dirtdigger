#!/usr/bin/python3
import sys
import time

from udpdata import *

def runTest():
    while True:
        time.sleep(5)
    
if __name__ == '__main__':
    print(" arg 0", sys.argv[0])
    for a in sys.argv:
        print(" arg ["+a+ "]")
        
    if len(sys.argv) > 1:
        print(" host", sys.argv[1])
    runTest()
    

