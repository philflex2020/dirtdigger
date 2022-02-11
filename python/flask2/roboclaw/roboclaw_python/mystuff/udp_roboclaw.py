import time
import threading
from roboclaw_3 import Roboclaw
import udpsystem as udps
lock = threading.Lock()
#Windows comport name
#rc = Roboclaw("COM9",115200)
#Linux comport name
rc = Roboclaw("/dev/serial0",38400)


def getrc():
    rC = {}
    lock.acquire()
    try:
        rC["enc1"] = rc.ReadEncM1(address)
        rC["enc2"] = rc.ReadEncM2(address)
        rC["speed1"] = rc.ReadSpeedM1(address)
        rC["speed2"] = rc.ReadSpeedM2(address)
    finally:
        lock.release()
    return rC

rc.Open()
address = 0x80

version = rc.ReadVersion(address)
if version[0]==False:
	print ("GETVERSION Failed")
else:
	print (repr(version[1]))


#scanTime = 0.5
#threadRun = True

#def rc_thread(key):
#    global scanTime
#    global threadRun
#    sock = None    
#    while(threadRun):
#    
#        rC = displayspeed()
#        msg = udps.getHostMsg("*", "set", key, rC)
#        print(msg)
#    
#        sock = udps.sockSend(sock, msg)
#        #print("\n")
#        time.sleep(scanTime)

#def timeMenu(cmds, data, dest):
#    global scanTime
#    if len(cmds) > 1:
#        udps.sendMsg(dest ,"setting scan time\n")
#        s_time =  float(cmds[1])
#        if((s_time > 0.1) and (s_time < 5.0)):
#            scanTime = s_time
#    else:
#        udps.sendMsg(dest ,"use [t time]\n")

def runMotor(cmds, data, dest):
    if len(cmds) > 3:
        
        speed = int(cmds[2])
        dist = int(cmds[3])
        buf = 0
        lock.acquire()
        try:
            if len(cmds) > 4:
            
                rc.SetEncM1(0x80, 0)
            rc.SpeedDistanceM1(0x80, speed, dist, 1)
        finally:
            lock.release()
        udps.sendMsg(dest ,"set [rm 1 speed dist [reset]]]\n")
    else:
        udps.sendMsg(dest ,"use [rm 1 speed dist [reset]]]\n")
        
if __name__ == "__main__":
    #global threadRun
    #udps.addMenuItem("t", "time", "alter scan time", timeMenu, None)
    udps.addMenuItem("rm", "run motor", " rm 1 speed dist [reset]", runMotor, None)
    #udps.runMenuInput(udps.udpMenu,"l 5679")
    udps.runService(None,["rc1",getrc], 5679)
    #x = threading.Thread(target=rc_thread, args=("rc1",))
    #x.start()
    #udps.runMenu()
    #threadRun = False
    #x.join()
   
# number direction [speed [ dist]]

#void motorRun(struct wConn &wc, void *data) {
#  uint32_t speed = 0;
#  uint32_t dist = 0;
#  String v_str = "";
#  if (wc.nwords > 2) {
#    speed = atoi((const char *)wc.swords[2]);
#  }
  
#  if (wc.nwords > 3) {
#    dist = atoi((const char *)wc.swords[3]);
#  }
  
#  v_str ="Speed:"+ String(speed)+ " Dist:"+ String(dist);
  
#  roboclaw.SetEncM1(0x80, 0);
#  if((speed>0) && (dist>0)) {
#     roboclaw.SpeedDistanceM1(0x80, speed, dist);
#
#  } else if((speed>0)) {
#    roboclaw.SpeedM1(0x80, speed);
#  }
#  v_str +="\n";

#  writeString(v_str, wc);
  
#}
