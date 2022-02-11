
import socket
import struct
import json
import sys
import threading
import time
import os
import platform
import udpsystem as udps


if __name__ == "__main__":
    #global hostname
    #hh = os.uname()
    #tmstart = cur_mtime()
    #gDict = uu.setupgDict()
    #udpmenu.setDict(gDict)    
    #g_acks = {}
    #g_groups = {}
    #g_tlist  = gDict["test_list"] 
    #g_tlist.append(1)
    #g_tlist.append(21)
    x = threading.Thread(target=udps.rport_thread, args=(udps.MCAST_RPORT,))
    x.start()
    #tstart = cur_utime()
    #sockSend(msg)
    x.join()
  
