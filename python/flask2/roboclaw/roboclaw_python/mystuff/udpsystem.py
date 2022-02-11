
import socket
import struct
import json
import sys
import select
import threading
import time
import os
import platform

cur_mtime = lambda: int(round(time.time() * 1000))
cur_utime = lambda: int(round(time.time() * 1000000))


MCAST_GRP = '224.1.1.1'
MCAST_CPORT = 5007
MCAST_RPORT = 5008
MULTICAST_TTL = 2

gDict = {}
tmbase = cur_mtime() -20

gDict["test_list"] = []
gDict["tmbase"] = tmbase
gDict["hn"] = platform.uname()

g_socks={}

rSocks = {}

g_threads = {}

sock = None

tstart = cur_utime()
tmstart = cur_mtime()
tmbase = 1579915173470
subHost=None
subNode=None
g_timeout  = 0;

def getAckIdHn():
    global gDict
    tmbase = gDict["tmbase"]
    hn = gDict["hn"]
    id  = (cur_mtime() - tmbase) %  100000
    return hn[1] + ":" + str(id)
# getHostMsg("*", "set", id = None):

def getHostMsg(towho, cmd, key, data, id = None):
    #global hn
    global gDict
    hn = gDict["hn"]
    md = {}
    msg = ""
    if towho != "-":
        if not id:
            id = getAckIdHn()
        md["host"] = towho
        if id != " ":
            md["id"] = id
        md["cmd"] = cmd
        md["send"] = hn[1]
        md["data"] = {}
        mdk = md["data"]
        mdk[key] = data
        msg = json.dumps(md)
        #del(msg[0])

        #msg = msg[1:-1]
        #msg += ","
    return msg


#'{"hostname":"*", "id":"821394900","cmd":"set","data::[{"ain":{"a0":"1.234","a1":"2.345"}}]}'
def sockSend(sock, msg):
    if sock == None :
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
        mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    sock.sendto(bytes(msg,'utf8'), (MCAST_GRP, MCAST_RPORT))
    return sock

def sockSend2(msg):
    global sock
    if(sock == None):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
        mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
        sock.sendto(bytes(msg), (MCAST_GRP, MCAST_RPORT))

def findNewSock(grp, port):
    global g_socks
    s = None
    if grp not in g_socks.keys():
        g_socks[grp] = {}
    gg = g_socks[grp]
    if port not in gg.keys():
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        gg[port] = s
    else:
        print ( " findNewSock already in use ", port) 
        #s = gg[port]
    return s

def createRsock(mgrp, rport):
    #rs = findNewSock(mgrp,rport)
    #if rs :
    rs = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    rs.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
    rs.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    mreq = struct.pack("4sl", socket.inet_aton(mgrp), socket.INADDR_ANY)
    rs.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    rs.bind((mgrp, rport))
    return rs

def runRsock(rsock, done):
    global g_timeout
    tval = 5

    decode = True
    try:
        print( "getting data\n")
        data = rsock.recv(10240)
        print( "received data\n")
    except socket.timeout:
        g_timeout += 1
        print (" timeout no data")
        if subNode == None and g_timeout > 3 :
            done = True
            print (" timeout : quitting")
        decode = False
    except:
        print(" recv error")
        decode = False
        
    if decode:
        rsock.settimeout(tval)
        tdone = cur_utime() - tstart
        
        print(" trip time ", tdone , "uS")
        print (data)

        try:
            jc = json.loads(data.decode('utf8'))
            print(" data decode JSON OK")
        except:
            print(" data decode error ignoring")
            decode = False
            

def rport_thread(rport):
    global tstart
    global tmstart
    global subHost
    done = False

    rsock = createRsock(MCAST_GRP, MCAST_RPORT)
    tval = 5.0
    rsock.settimeout(tval)
    while not done:
        done = runRsock(rsock, done)
# hack for windows
#import msvcrt
# if msvcrt.kbhit(): readsocks.append(sys.stdin)


udpMenu = {}
udpKeys = {}
udpMenuCtl = {}
udpMenuCtl["tick"] = 0
udpMenuCtl["quit"] = 0
udpMenuCtl["menu"] = {}
menuSocks = [ sys.stdin,]
menuServers = [ ]

def runMenu(dest=sys.stdin, data= None):
    global menuSocks
    global menuServers
    global udpMenu
    global rSocks
    
    if dest == None:
        dest=sys.stdin
        
    if len(udpMenu) == 0:
        setup()
    

    while udpMenuCtl["quit"] == 0:
        pmsg = ""
        if not data:
            pmsg = "udpmenu >"
        else:
            pmsg = data + " >"
        #if(dest == sys.stdin):
        #    sendMsg(dest, pmsg)
        mflag = 0
        while not mflag:
            read_socks,write_socks,error_socks = select.select(menuSocks,[],[],0.01)
            udpMenuCtl["tick"] = udpMenuCtl["tick"] + 1
            #print ("tick")
            for s in read_socks:
                if s == sys.stdin:
                    mflag = 1;
                    cmd = s.readline()
                    runMenuInput(udpMenu, cmd, sys.stdout, data)
                    sendMsg(s, pmsg)
                    
                elif s in menuServers:
                    #print("server accept\n")
                    #mflag = 1;
                    con, ca = s.accept()
                    con.setblocking(0)
                    menuSocks.append(con)

                elif s in rSocks.keys():
                    #print (" rSocks input \n")
                    #mflag = 1;
                    d = rSocks[s]
                    decoder = d["decoder"]
                    dest = d["dest"]
                    fcn = d["read"]
                    dat = d["data"]
                    rc = fcn(s, dat, dest, decoder)
                
                elif s in menuSocks:
                    #print (" menusocks input \n")
                    mflag = 1;
                    cmd = s.recv(1024)
                    c = cmd.decode("utf-8")
                    #print(c)
                    #print("\n")
                    #sendMsg(s," running cmd\n")
                    runMenuInput(udpMenu, c, s, data)
                    #runMenuInput(udpMenu,cmd, s, data)
                    sendMsg(s, pmsg)
                    
def addSocket(cmds, data, dest):
    global menuSocks
    print (" len cmds ", len(cmds))
    if len(cmds) > 1:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.setblocking(0)
        sa = ('localhost',int(cmds[1]))
        s.bind(sa)
        s.listen(5)
        menuSocks.append(s)
        menuServers.append(s)
        print(s)
    else:
        sendMsg(dest," please provide a port\n")
    
def msendSock(a, b, dest=None):
    global mgrp
    if len(a) > 2:
        port =  a[1]
        msg =  a[2]
        uu.sockSend(a[2], mgrp, int(a[1]))
            
def addMenuItem(key, name, desc, fcn, data=None, resp = None):
    global udpMenu
    if len(udpMenu) == 0:
        setup()
    setMenuItem(key, name, desc, fcn, data, resp)

def addMenuTestItem(key, name, desc , fcn, resp):
    return addMenuItem(key, name, desc, fcn, sys.stdout, resp)
    
def setMenuItem(key, name, desc, fcn, data=None, resp=None):
    global udpMenu
    global udpKeys
    d = {}
    d["desc"]=desc
    d["fcn"]=fcn
    d["data"]=data
    d["key"]=key
    d["resp"]=resp
    udpMenu[name] = d
    udpKeys[key] = d

def addChar(v,n):
    #if n in ['\'','\"']:
    #    v += '\\'
    v += n
    #print(v ,n)
    return v

def mysplit(cmd):
    res = []
    esc = None
    v = ""
    for n in cmd:
        if n == " ":
            if not esc:
                if len(v) > 0:
                    res.append(v)
                    v = ""
            else:
                v += n       
        elif n in ['"','\'']:
            if n == esc:
                esc = None
            else:
                if esc:
                    v = addChar(v,n)
                    #v += n
                else:
                    esc = n
        elif n == '\n':
            if not esc:
                if len(v) > 0:
                    res.append(v)
                    v = ""
            else:
                v = addChar(v,n)
        elif n == '\r':
            continue
        else:
            v += n
    if len(v) > 0:
        res.append(v)
        v = ""
    return res
    
def runMenuInput(gMenu, cmd, dest=sys.stdout, data = None):
    #global udpMenu
    cmds = mysplit(cmd)
    #print(" cmds ")
    #print(cmds)
    return runMenuCmds(gMenu, cmds, dest, data)
    
def runMenuCmds(gMenu, cmds, dest, data = None):
    global udpMenu
    global udpKeys
    d = None
    if len(cmds) > 0:
        if  cmds[0] in udpKeys.keys():
            #print (" found d in udpKeys\n")
            d = udpKeys[cmds[0]]
            if  cmds[0] in gMenu.keys():
                #print (" found d in gMenuKeys\n")
                d = gMenu[cmds[0]]
    if d:
        #print (" found cmd\n")
        #print (dest)
        #sendMsg(dest, " running cmd 2 \n")
        d["fcn"] (cmds, d["resp"], dest)
    else:
        print (" no cmd found\n")
        
def showMenuItems(cmds, data, dest):
    global udpMenu
    for k in udpMenu.keys():
        d = udpMenu[k]
        msg = "("+d["key"]+") "+ k +": " + d["desc"] +"\n"
        sendMsg(dest,msg)

def sendMsg(dest,msg):
    #print(dest)
    if dest == sys.stdin:
        sys.stdout.write(msg)
        sys.stdout.flush()
    elif dest == sys.stdout:
        sys.stdout.write(msg)
        sys.stdout.flush()
    else:
        dest.send( msg.encode("utf-8"))

def sendMsgNl(dest,msg):
    sendMsg(dest,msg)
    sendMsg(dest,"\n")

def setup():
    setMenuItem("h", "help", " Show menu options", showMenuItems)
    setMenuItem("q", "quit", " Quit", quitMenu)
    setMenuItem("l", "listen", "Add Listen Port", addSocket)
    addMenuItem("t", "test", " run a test item", testMenu, None)
    
def setDict(dict):
    global gDict
    gDict = dict

def getGmenu():
    global udpMenu
    return udpMenu    
    
def testMenu(cmds, data, dest):
    global udpMenu
    global gDict
    gDict["cmds"] = cmds
    sendMsg( dest, "running test menu\n")

def quitMenu(cmds, data, dest):
    global udpMenu
    sendMsg(dest ,"running quit menu\n")
    udpMenuCtl["quit"] = 1

        
   
def timeMenu(cmds, data, dest):
    global scanTime
    if len(cmds) > 1:
        sendMsg(dest ,"setting scan time\n")
        s_time =  float(cmds[1])
        if((s_time > 0.1) and (s_time < 5.0)):
            scanTime = s_time
    else:
        sendMsg(dest ,"use [t time]\n")

threadRun = True
mysock = None

def defThread(args):
    global scanTime
    global threadRun
    global mysock

    while(threadRun):
        # run optional loop task
        if len(args) > 2:
            if len(args) > 3:
                args[2](args[3])
            else :
                args[2]()
                
        rC = args[1]()
        msg = getHostMsg("*","set",args[0], rC)
        print (msg)
        mysock = sockSend(mysock, msg)
        time.sleep(scanTime)
    print ("Thread done")

def sendEvent(hosts,evnt,args,rC):
    global mysock
    msg = getHostMsg("*",evnt,args, rC)
    print (msg)
    mysock = sockSend(mysock, msg)
    

def tManage(tName):
    global g_threads
    thDict = g_threads[tName]
    args = thDict["args"]
    
    lfcn = args[1]
    lint = args[2]
    ldata = args[3]

    while(thDict["run"]==1):
        lfcn(ldata)
        time.sleep(lint)
        
        
def runService(threadFcn, xargs, port=0, stime=0.5) :
    global threadRun
    global scanTime
    scanTime = stime
    if threadFcn == None:
        threadFcn = defThread
    addMenuItem("t", "time", "alter scan time", timeMenu, None)
    if port > 0:
        msg = "l " + str(port)
        runMenuInput(udpMenu,msg)
    #runMenuInput(udpMenu,"t 0.5")
    threadRun = True
    thrd = threading.Thread(target=threadFcn, args=(xargs,))
    thrd.start()
    runMenu()
    threadRun = False
    thrd.join()

def stopTask(tName):
    global g_threads
    g_threads[tName]["run"] = 0;
    thrd = g_threads[tName]["thrd"];
    thrd.join()
    
def runTask(tName, tFcn, interval, data):
    global g_threads
    thrd = threading.Thread(target=tManage, args=(tName,))
    g_threads[tName] = {}
    g_threads[tName]["args"] = (tName, tFcn, interval, data)
    g_threads[tName]["run"] = 1;
    g_threads[tName]["thrd"] = thrd;
    thrd.start()

    
if __name__ == "__main__":
    print("hello\n")
    addMenuItem("t", "test", " run a test item", testMenu, None)
    runMenuInput(udpMenu,"test the menu", sys.stdout)
    runMenuInput(udpMenu, "help the menu", sys.stdout)
    #runMenuInput("quit the menu", sys.stdout)
    runMenu(sys.stdin)
    print("bye  : quit",   udpMenuCtl["quit"] , "\n")

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
    #x = threading.Thread(target=rport_thread, args=(MCAST_RPORT,))
    #x.start()
    #tstart = cur_utime()
    #sockSend(msg)
    #x.join()
  
